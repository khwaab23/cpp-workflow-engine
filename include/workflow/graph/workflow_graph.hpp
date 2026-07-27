#pragma once

#include <unordered_map>
#include <vector>

#include "workflow/workflow_definition.hpp"

namespace workflow {

class WorkflowGraph {
 public:
  explicit WorkflowGraph(WorkflowDefinition definition);

  [[nodiscard]] const WorkflowDefinition& definition() const noexcept {
    return definition_;
  }
  [[nodiscard]] const std::vector<TaskId>& dependentsOf(
      const TaskId& id) const;

 private:
  WorkflowDefinition definition_;
  std::unordered_map<TaskId, std::vector<TaskId>> dependents_;
};

}  // namespace workflow
