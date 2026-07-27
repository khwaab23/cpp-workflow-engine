#include "workflow/workflow_engine.hpp"

#include <chrono>
#include <future>
#include <stdexcept>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "workflow/graph/workflow_graph.hpp"
#include "workflow/graph/workflow_validator.hpp"
#include "workflow/observability/metrics.hpp"
#include "workflow/runtime/scheduler.hpp"
#include "workflow/runtime/thread_pool.hpp"
#include "workflow/task_state.hpp"

namespace workflow {
namespace {

struct TaskOutcome {
  TaskResult result;
  std::size_t attempts{0};
};

}  // namespace

WorkflowEngine::WorkflowEngine(WorkflowDefinition definition,
                               std::unique_ptr<IStateStore> stateStore,
                               std::size_t workerCount, Logger* logger)
    : definition_(std::move(definition)),
      stateStore_(std::move(stateStore)),
      workerCount_(workerCount),
      logger_(logger) {
  if (!stateStore_) {
    throw std::invalid_argument("state store is required");
  }
  if (workerCount_ == 0) {
    throw std::invalid_argument("worker count must be positive");
  }
}

WorkflowResult WorkflowEngine::execute() {
  const auto validation = WorkflowValidator{}.validate(definition_);
  if (!validation.valid()) {
    throw std::invalid_argument(validation.errors.front());
  }

  const auto started = std::chrono::steady_clock::now();
  WorkflowGraph graph(definition_);
  std::unordered_set<TaskId> restoredSucceeded;
  WorkflowSnapshot snapshot{definition_.id(), {}};
  if (const auto stored = stateStore_->load(definition_.id())) {
    for (const auto& [id, task] : stored->tasks) {
      if (!definition_.containsTask(id)) {
        throw std::runtime_error("snapshot contains an unknown task: " + id);
      }
      if (task.state == TaskState::Succeeded) {
        restoredSucceeded.insert(id);
      }
    }
    snapshot = *stored;
  }
  Scheduler scheduler(graph, restoredSucceeded);
  ThreadPool pool(workerCount_);
  MetricsCollector metrics;
  std::unordered_map<TaskId, TaskExecutionState> states;
  std::unordered_map<TaskId, std::future<TaskOutcome>> running;

  for (const auto& [id, task] : definition_.tasks()) {
    (void)task;
    auto& saved = snapshot.tasks[id];
    auto& state = states[id];
    if (restoredSucceeded.contains(id)) {
      state.transitionTo(TaskState::Ready);
      state.transitionTo(TaskState::Running);
      state.transitionTo(TaskState::Succeeded);
      metrics.recordSuccess();
    } else {
      saved = TaskSnapshot{};
    }
  }

  std::size_t completed = restoredSucceeded.size();
  while (completed < definition_.tasks().size()) {
    while (scheduler.hasReadyTask()) {
      auto id = scheduler.takeReadyTask();
      auto& state = states.at(id);
      state.transitionTo(TaskState::Ready);
      snapshot.tasks.at(id).state = TaskState::Ready;
      state.transitionTo(TaskState::Running);
      snapshot.tasks.at(id).state = TaskState::Running;
      const Task* task = &definition_.task(id);
      if (logger_ != nullptr) {
        logger_->log(LogLevel::Info, "starting task " + id);
      }
      running.emplace(
          id, pool.submit([task, &metrics] {
            TaskOutcome outcome;
            const auto& policy = task->retryPolicy();
            for (;;) {
              ++outcome.attempts;
              outcome.result = task->execute();
              if (outcome.result.success ||
                  outcome.attempts > policy.maxRetries) {
                return outcome;
              }
              metrics.recordRetry();
              std::this_thread::sleep_for(
                  policy.backoffForAttempt(outcome.attempts));
            }
          }));
    }

    if (running.empty()) {
      throw std::logic_error("scheduler made no progress");
    }

    auto current = running.begin();
    auto id = current->first;
    auto outcome = current->second.get();
    running.erase(current);
    auto& taskSnapshot = snapshot.tasks.at(id);
    taskSnapshot.attempts = outcome.attempts;
    if (outcome.result.success) {
      states.at(id).transitionTo(TaskState::Succeeded);
      taskSnapshot.state = TaskState::Succeeded;
      metrics.recordSuccess();
      scheduler.onTaskSucceeded(id);
      if (logger_ != nullptr) {
        logger_->log(LogLevel::Info, "task succeeded: " + id);
      }
    } else {
      states.at(id).transitionTo(TaskState::Failed);
      taskSnapshot.state = TaskState::Failed;
      taskSnapshot.error = std::move(outcome.result.error);
      metrics.recordFailure();
      if (logger_ != nullptr) {
        logger_->log(LogLevel::Error, "task failed: " + id);
      }
      for (const auto& skipped : scheduler.onTaskFailed(id)) {
        auto& skippedSnapshot = snapshot.tasks.at(skipped);
        if (skippedSnapshot.state == TaskState::Pending) {
          states.at(skipped).transitionTo(TaskState::Skipped);
          skippedSnapshot.state = TaskState::Skipped;
          skippedSnapshot.error = "dependency failed";
          metrics.recordSkipped();
          ++completed;
        }
      }
    }
    ++completed;
    stateStore_->save(snapshot);
  }

  auto finalMetrics = metrics.snapshot();
  finalMetrics.elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::steady_clock::now() - started);
  return {finalMetrics.failed == 0, std::move(snapshot), finalMetrics};
}

}  // namespace workflow
