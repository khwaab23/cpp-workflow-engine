#include "test_framework.hpp"

#include <atomic>
#include <mutex>
#include <optional>

#include "workflow/storage/state_store.hpp"
#include "workflow/workflow_engine.hpp"

using namespace workflow;

namespace {
class MemoryStore final : public IStateStore {
 public:
  void save(const WorkflowSnapshot& value) override {
    std::lock_guard lock(mutex_);
    snapshot_ = value;
  }
  std::optional<WorkflowSnapshot> load(const WorkflowId&) override {
    std::lock_guard lock(mutex_);
    return snapshot_;
  }

 private:
  std::mutex mutex_;
  std::optional<WorkflowSnapshot> snapshot_;
};

class SeededStore final : public IStateStore {
 public:
  explicit SeededStore(WorkflowSnapshot snapshot)
      : snapshot_(std::move(snapshot)) {}
  void save(const WorkflowSnapshot& value) override { snapshot_ = value; }
  std::optional<WorkflowSnapshot> load(const WorkflowId&) override {
    return snapshot_;
  }

 private:
  WorkflowSnapshot snapshot_;
};
}  // namespace

TEST(engine_executes_dag_and_retries) {
  std::atomic<int> attempts{0};
  WorkflowDefinition definition("success");
  definition.addTask(
      Task("root", "Root", {}, [] { return TaskResult::succeeded(); }));
  definition.addTask(Task(
      "retry", "Retry", {"root"},
      [&] {
        return ++attempts == 1 ? TaskResult::failed("transient")
                               : TaskResult::succeeded();
      },
      RetryPolicy{1, std::chrono::milliseconds{0}, 1.0}));
  WorkflowEngine engine(std::move(definition),
                        std::make_unique<MemoryStore>(), 2);
  const auto result = engine.execute();
  REQUIRE(result.success);
  REQUIRE(result.metrics.succeeded == 2);
  REQUIRE(result.metrics.retries == 1);
  REQUIRE(result.snapshot.tasks.at("retry").attempts == 2);
}

TEST(engine_skips_transitive_dependents) {
  WorkflowDefinition definition("failure");
  definition.addTask(
      Task("a", "A", {}, [] { return TaskResult::failed("boom"); }));
  definition.addTask(
      Task("b", "B", {"a"}, [] { return TaskResult::succeeded(); }));
  definition.addTask(
      Task("c", "C", {"b"}, [] { return TaskResult::succeeded(); }));
  WorkflowEngine engine(std::move(definition),
                        std::make_unique<MemoryStore>(), 2);
  const auto result = engine.execute();
  REQUIRE(!result.success);
  REQUIRE(result.metrics.failed == 1);
  REQUIRE(result.metrics.skipped == 2);
  REQUIRE(result.snapshot.tasks.at("c").state == TaskState::Skipped);
}

TEST(engine_resumes_succeeded_tasks) {
  std::atomic<int> rootRuns{0};
  WorkflowDefinition definition("resume");
  definition.addTask(Task("a", "A", {}, [&] {
    ++rootRuns;
    return TaskResult::succeeded();
  }));
  definition.addTask(
      Task("b", "B", {"a"}, [] { return TaskResult::succeeded(); }));
  WorkflowSnapshot seed{"resume",
                        {{"a", {TaskState::Succeeded, 1, {}}},
                         {"b", {TaskState::Pending, 0, {}}}}};
  WorkflowEngine engine(std::move(definition),
                        std::make_unique<SeededStore>(std::move(seed)), 1);
  const auto result = engine.execute();
  REQUIRE(result.success);
  REQUIRE(rootRuns == 0);
  REQUIRE(result.metrics.succeeded == 2);
}
