#pragma once

#include <cstddef>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "workflow/graph/workflow_graph.hpp"
#include "workflow/state/workflow_snapshot.hpp"

namespace workflow {

class Scheduler {
 public:
  explicit Scheduler(
      const WorkflowGraph& graph,
      const std::unordered_set<TaskId>& alreadySucceeded = {});
  [[nodiscard]] bool hasReadyTask() const noexcept { return !ready_.empty(); }
  TaskId takeReadyTask();
  void onTaskSucceeded(const TaskId& id);
  [[nodiscard]] std::vector<TaskId> onTaskFailed(const TaskId& id);

 private:
  const WorkflowGraph& graph_;
  std::unordered_set<TaskId> alreadySucceeded_;
  std::unordered_map<TaskId, std::size_t> remainingDependencies_;
  std::deque<TaskId> ready_;
};

}  // namespace workflow
