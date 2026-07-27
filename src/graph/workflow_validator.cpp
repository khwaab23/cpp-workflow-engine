#include "workflow/graph/workflow_validator.hpp"

#include <functional>
#include <unordered_map>
#include <unordered_set>

namespace workflow {

ValidationResult WorkflowValidator::validate(
    const WorkflowDefinition& definition) const {
  ValidationResult result;
  if (definition.tasks().empty()) {
    result.errors.emplace_back("workflow must contain at least one task");
    return result;
  }

  for (const auto& [id, task] : definition.tasks()) {
    std::unordered_set<TaskId> seen;
    for (const auto& dependency : task.dependencies()) {
      if (!definition.containsTask(dependency)) {
        result.errors.push_back("task '" + id +
                                "' has missing dependency '" + dependency + "'");
      }
      if (dependency == id) {
        result.errors.push_back("task '" + id + "' depends on itself");
      }
      if (!seen.insert(dependency).second) {
        result.errors.push_back("task '" + id +
                                "' repeats dependency '" + dependency + "'");
      }
    }
  }
  if (!result.errors.empty()) {
    return result;
  }

  enum class Mark { Unvisited, Visiting, Visited };
  std::unordered_map<TaskId, Mark> marks;
  std::function<bool(const TaskId&)> visit = [&](const TaskId& id) {
    if (marks[id] == Mark::Visiting) {
      return false;
    }
    if (marks[id] == Mark::Visited) {
      return true;
    }
    marks[id] = Mark::Visiting;
    for (const auto& dependency : definition.task(id).dependencies()) {
      if (!visit(dependency)) {
        return false;
      }
    }
    marks[id] = Mark::Visited;
    return true;
  };
  for (const auto& [id, task] : definition.tasks()) {
    (void)task;
    if (!visit(id)) {
      result.errors.emplace_back("workflow graph contains a cycle");
      break;
    }
  }
  return result;
}

}  // namespace workflow
