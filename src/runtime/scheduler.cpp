#include "workflow/runtime/scheduler.hpp"

#include <queue>
#include <stdexcept>
#include <unordered_set>

namespace workflow {

Scheduler::Scheduler(
    const WorkflowGraph& graph,
    const std::unordered_set<TaskId>& alreadySucceeded)
    : graph_(graph), alreadySucceeded_(alreadySucceeded) {
  for (const auto& [id, task] : graph_.definition().tasks()) {
    std::size_t count = 0;
    for (const auto& dependency : task.dependencies()) {
      if (!alreadySucceeded.contains(dependency)) {
        ++count;
      }
    }
    remainingDependencies_.emplace(id, count);
    if (count == 0 && !alreadySucceeded.contains(id)) {
      ready_.push_back(id);
    }
  }
}

TaskId Scheduler::takeReadyTask() {
  if (ready_.empty()) {
    throw std::logic_error("no task is ready");
  }
  auto id = std::move(ready_.front());
  ready_.pop_front();
  return id;
}

void Scheduler::onTaskSucceeded(const TaskId& id) {
  for (const auto& dependent : graph_.dependentsOf(id)) {
    auto& remaining = remainingDependencies_.at(dependent);
    if (remaining > 0 && --remaining == 0 &&
        !alreadySucceeded_.contains(dependent)) {
      ready_.push_back(dependent);
    }
  }
}

std::vector<TaskId> Scheduler::onTaskFailed(const TaskId& id) {
  std::vector<TaskId> skipped;
  std::queue<TaskId> pending;
  std::unordered_set<TaskId> seen;
  for (const auto& child : graph_.dependentsOf(id)) {
    pending.push(child);
  }
  while (!pending.empty()) {
    auto current = std::move(pending.front());
    pending.pop();
    if (!seen.insert(current).second) {
      continue;
    }
    skipped.push_back(current);
    for (const auto& child : graph_.dependentsOf(current)) {
      pending.push(child);
    }
  }
  return skipped;
}

}  // namespace workflow
