#pragma once

#include <filesystem>
#include <mutex>

#include "workflow/storage/state_store.hpp"

namespace workflow {

// Thread-safe. Each save atomically replaces one workflow state file.
class FileStateStore final : public IStateStore {
 public:
  explicit FileStateStore(std::filesystem::path directory);
  void save(const WorkflowSnapshot& snapshot) override;
  [[nodiscard]] std::optional<WorkflowSnapshot> load(
      const WorkflowId& workflowId) override;

 private:
  [[nodiscard]] std::filesystem::path pathFor(const WorkflowId& id) const;
  std::filesystem::path directory_;
  std::mutex mutex_;
};

}  // namespace workflow
