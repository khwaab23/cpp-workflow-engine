#pragma once

#include <stdexcept>

namespace workflow {

enum class TaskState {
  Pending,
  Ready,
  Running,
  Retrying,
  Succeeded,
  Failed,
  Skipped
};

class TaskExecutionState {
 public:
  [[nodiscard]] TaskState value() const noexcept { return state_; }
  void transitionTo(TaskState next);

 private:
  TaskState state_{TaskState::Pending};
};

}  // namespace workflow
