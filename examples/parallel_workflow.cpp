#include <memory>
#include "workflow/storage/file_state_store.hpp"
#include "workflow/workflow_engine.hpp"

int main() {
  workflow::WorkflowDefinition definition("parallel");
  for (const auto* id : {"left", "right"}) {
    definition.addTask(workflow::Task(
        id, id, {}, [] { return workflow::TaskResult::succeeded(); }));
  }
  workflow::WorkflowEngine engine(
      std::move(definition),
      std::make_unique<workflow::FileStateStore>(".workflow-state"), 2);
  return engine.execute().success ? 0 : 1;
}
