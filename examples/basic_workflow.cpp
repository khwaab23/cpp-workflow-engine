#include <iostream>
#include <memory>

#include "workflow/storage/file_state_store.hpp"
#include "workflow/workflow_engine.hpp"

int main() {
  workflow::WorkflowDefinition workflow("example");
  workflow.addTask(workflow::Task(
      "fetch", "Fetch input", {},
      [] {
        std::cout << "fetching\n";
        return workflow::TaskResult::succeeded();
      }));
  workflow.addTask(workflow::Task(
      "transform", "Transform input", {"fetch"},
      [] {
        std::cout << "transforming\n";
        return workflow::TaskResult::succeeded();
      }));
  workflow::WorkflowEngine engine(
      std::move(workflow),
      std::make_unique<workflow::FileStateStore>(".workflow-state"), 2);
  return engine.execute().success ? 0 : 1;
}
