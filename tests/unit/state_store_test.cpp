#include "test_framework.hpp"

#include <chrono>
#include <filesystem>

#include "workflow/storage/file_state_store.hpp"

using namespace workflow;

TEST(file_state_store_round_trips_snapshot) {
  const auto unique = std::to_string(
      std::chrono::steady_clock::now().time_since_epoch().count());
  const auto directory =
      std::filesystem::temp_directory_path() / ("workflow-test-" + unique);
  {
    FileStateStore store(directory);
    WorkflowSnapshot expected{
        "roundtrip",
        {{"task one", {TaskState::Failed, 3, "quoted \"error\""}}}};
    store.save(expected);
    const auto actual = store.load("roundtrip");
    REQUIRE(actual.has_value());
    REQUIRE(actual->tasks.at("task one").state == TaskState::Failed);
    REQUIRE(actual->tasks.at("task one").attempts == 3);
    REQUIRE(actual->tasks.at("task one").error == "quoted \"error\"");
  }
  std::filesystem::remove_all(directory);
}
