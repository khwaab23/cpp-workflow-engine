#pragma once

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "workflow/task.hpp"

namespace workflow {

class WorkflowDefinition {
 public:
  explicit WorkflowDefinition(WorkflowId id);
  void addTask(Task task);

  [[nodiscard]] const WorkflowId& id() const noexcept { return id_; }
  [[nodiscard]] const Task& task(const TaskId& id) const;
  [[nodiscard]] bool containsTask(const TaskId& id) const noexcept;
  [[nodiscard]] const std::unordered_map<TaskId, Task>& tasks() const noexcept {
    return tasks_;
  }

 private:
  WorkflowId id_;
  std::unordered_map<TaskId, Task> tasks_;
};

}  // namespace workflow
