#pragma once

#include <functional>
#include <string>
#include <vector>

#include "workflow/retry_policy.hpp"
#include "workflow/task_result.hpp"

namespace workflow {

using TaskId = std::string;
using WorkflowId = std::string;
using TaskFunction = std::function<TaskResult()>;

class Task {
 public:
  Task(TaskId id, std::string name, std::vector<TaskId> dependencies,
       TaskFunction function, RetryPolicy retryPolicy = {});

  [[nodiscard]] const TaskId& id() const noexcept { return id_; }
  [[nodiscard]] const std::string& name() const noexcept { return name_; }
  [[nodiscard]] const std::vector<TaskId>& dependencies() const noexcept {
    return dependencies_;
  }
  [[nodiscard]] const RetryPolicy& retryPolicy() const noexcept {
    return retryPolicy_;
  }
  [[nodiscard]] TaskResult execute() const;

 private:
  TaskId id_;
  std::string name_;
  std::vector<TaskId> dependencies_;
  TaskFunction function_;
  RetryPolicy retryPolicy_;
};

}  // namespace workflow
