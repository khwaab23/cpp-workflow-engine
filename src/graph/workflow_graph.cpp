#include "workflow/graph/workflow_graph.hpp"

#include <stdexcept>
#include <utility>

namespace workflow {

WorkflowGraph::WorkflowGraph(WorkflowDefinition definition)
    : definition_(std::move(definition)) {
  for (const auto& [id, task] : definition_.tasks()) {
    dependents_.try_emplace(id);
    for (const auto& dependency : task.dependencies()) {
      dependents_[dependency].push_back(id);
    }
  }
}

const std::vector<TaskId>& WorkflowGraph::dependentsOf(
    const TaskId& id) const {
  const auto found = dependents_.find(id);
  if (found == dependents_.end()) {
    throw std::out_of_range("unknown task id: " + id);
  }
  return found->second;
}

}  // namespace workflow
