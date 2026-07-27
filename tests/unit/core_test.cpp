#include "test_framework.hpp"

#include <atomic>

#include "workflow/graph/workflow_graph.hpp"
#include "workflow/graph/workflow_validator.hpp"
#include "workflow/runtime/scheduler.hpp"
#include "workflow/runtime/thread_pool.hpp"
#include "workflow/task_state.hpp"

using namespace workflow;

TEST(validator_rejects_missing_dependency) {
  WorkflowDefinition definition("invalid");
  definition.addTask(
      Task("a", "A", {"missing"}, [] { return TaskResult::succeeded(); }));
  REQUIRE(!WorkflowValidator{}.validate(definition).valid());
}

TEST(validator_rejects_cycles) {
  WorkflowDefinition definition("cycle");
  definition.addTask(
      Task("a", "A", {"b"}, [] { return TaskResult::succeeded(); }));
  definition.addTask(
      Task("b", "B", {"a"}, [] { return TaskResult::succeeded(); }));
  REQUIRE(!WorkflowValidator{}.validate(definition).valid());
}

TEST(scheduler_releases_dependents) {
  WorkflowDefinition definition("dag");
  definition.addTask(
      Task("a", "A", {}, [] { return TaskResult::succeeded(); }));
  definition.addTask(
      Task("b", "B", {"a"}, [] { return TaskResult::succeeded(); }));
  WorkflowGraph graph(std::move(definition));
  Scheduler scheduler(graph);
  REQUIRE(scheduler.takeReadyTask() == "a");
  scheduler.onTaskSucceeded("a");
  REQUIRE(scheduler.takeReadyTask() == "b");
}

TEST(state_machine_rejects_invalid_transition) {
  TaskExecutionState state;
  bool threw = false;
  try {
    state.transitionTo(TaskState::Succeeded);
  } catch (const std::logic_error&) {
    threw = true;
  }
  REQUIRE(threw);
}

TEST(thread_pool_executes_jobs) {
  ThreadPool pool(2);
  auto first = pool.submit([] { return 20; });
  auto second = pool.submit([] { return 22; });
  REQUIRE(first.get() + second.get() == 42);
}
