#include <atomic>
#include <memory>
#include "workflow/storage/file_state_store.hpp"
#include "workflow/workflow_engine.hpp"

int main() {
  std::atomic<int> attempts{0};
  workflow::WorkflowDefinition definition("retry");
  definition.addTask(workflow::Task(
      "unstable", "Unstable", {},
      [&] {
        return ++attempts < 3 ? workflow::TaskResult::failed("try again")
                              : workflow::TaskResult::succeeded();
      },
      {2, std::chrono::milliseconds{10}, 2.0}));
  workflow::WorkflowEngine engine(
      std::move(definition),
      std::make_unique<workflow::FileStateStore>(".workflow-state"), 1);
  return engine.execute().success ? 0 : 1;
}
