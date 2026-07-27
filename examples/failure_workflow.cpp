#include <memory>
#include "workflow/storage/file_state_store.hpp"
#include "workflow/workflow_engine.hpp"

int main() {
  workflow::WorkflowDefinition definition("failure");
  definition.addTask(workflow::Task(
      "fails", "Fails", {},
      [] { return workflow::TaskResult::failed("intentional"); }));
  definition.addTask(workflow::Task(
      "skipped", "Skipped", {"fails"},
      [] { return workflow::TaskResult::succeeded(); }));
  workflow::WorkflowEngine engine(
      std::move(definition),
      std::make_unique<workflow::FileStateStore>(".workflow-state"), 1);
  return engine.execute().success ? 0 : 1;
}
