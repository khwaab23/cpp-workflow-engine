#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <unordered_map>

#include "workflow/task.hpp"
#include "workflow/task_state.hpp"

namespace workflow {

struct TaskSnapshot {
  TaskState state{TaskState::Pending};
  std::size_t attempts{0};
  std::string error;
};

struct WorkflowSnapshot {
  WorkflowId workflowId;
  std::unordered_map<TaskId, TaskSnapshot> tasks;
};

}  // namespace workflow
