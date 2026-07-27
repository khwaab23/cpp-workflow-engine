#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>

#include "workflow/storage/state_store.hpp"
#include "workflow/workflow_engine.hpp"

namespace {
class NullStore final : public workflow::IStateStore {
 public:
  void save(const workflow::WorkflowSnapshot&) override {}
  std::optional<workflow::WorkflowSnapshot> load(
      const workflow::WorkflowId&) override {
    return std::nullopt;
  }
};
}  // namespace

int main() {
  workflow::WorkflowDefinition definition("benchmark");
  for (int index = 0; index < 1000; ++index) {
    definition.addTask(workflow::Task(
        std::to_string(index), "task", {},
        [] { return workflow::TaskResult::succeeded(); }));
  }
  const auto start = std::chrono::steady_clock::now();
  workflow::WorkflowEngine engine(std::move(definition),
                                  std::make_unique<NullStore>(), 8);
  const auto result = engine.execute();
  const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::steady_clock::now() - start);
  std::cout << result.metrics.succeeded << " tasks in " << elapsed.count()
            << " ms\n";
}
