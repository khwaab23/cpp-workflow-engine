#include "workflow/task.hpp"

#include <exception>
#include <stdexcept>
#include <utility>

#include "workflow/task_state.hpp"
#include "workflow/workflow_definition.hpp"

namespace workflow {

Task::Task(TaskId id, std::string name, std::vector<TaskId> dependencies,
           TaskFunction function, RetryPolicy retryPolicy)
    : id_(std::move(id)),
      name_(std::move(name)),
      dependencies_(std::move(dependencies)),
      function_(std::move(function)),
      retryPolicy_(retryPolicy) {
  if (id_.empty() || name_.empty() || !function_) {
    throw std::invalid_argument("task id, name, and function are required");
  }
}

TaskResult Task::execute() const {
  try {
    return function_();
  } catch (const std::exception& exception) {
    return TaskResult::failed(exception.what());
  } catch (...) {
    return TaskResult::failed("unknown task exception");
  }
}

void TaskExecutionState::transitionTo(TaskState next) {
  const bool valid =
      (state_ == TaskState::Pending &&
       (next == TaskState::Ready || next == TaskState::Skipped)) ||
      (state_ == TaskState::Ready &&
       (next == TaskState::Running || next == TaskState::Skipped)) ||
      (state_ == TaskState::Running &&
       (next == TaskState::Succeeded || next == TaskState::Failed ||
        next == TaskState::Retrying)) ||
      (state_ == TaskState::Retrying && next == TaskState::Running);
  if (!valid) {
    throw std::logic_error("invalid task state transition");
  }
  state_ = next;
}

WorkflowDefinition::WorkflowDefinition(WorkflowId id) : id_(std::move(id)) {
  if (id_.empty()) {
    throw std::invalid_argument("workflow id is required");
  }
}

void WorkflowDefinition::addTask(Task task) {
  const auto id = task.id();
  if (!tasks_.emplace(id, std::move(task)).second) {
    throw std::invalid_argument("duplicate task id: " + id);
  }
}

const Task& WorkflowDefinition::task(const TaskId& id) const {
  const auto found = tasks_.find(id);
  if (found == tasks_.end()) {
    throw std::out_of_range("unknown task id: " + id);
  }
  return found->second;
}

bool WorkflowDefinition::containsTask(const TaskId& id) const noexcept {
  return tasks_.contains(id);
}

}  // namespace workflow
