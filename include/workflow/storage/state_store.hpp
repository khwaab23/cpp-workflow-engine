#pragma once

#include <optional>

#include "workflow/state/workflow_snapshot.hpp"

namespace workflow {

class IStateStore {
 public:
  virtual ~IStateStore() = default;
  virtual void save(const WorkflowSnapshot& snapshot) = 0;
  [[nodiscard]] virtual std::optional<WorkflowSnapshot> load(
      const WorkflowId& workflowId) = 0;
};

}  // namespace workflow
