#pragma once

#include <string>
#include <utility>

namespace workflow {

struct TaskResult {
  bool success{true};
  std::string error;

  static TaskResult succeeded() { return {}; }
  static TaskResult failed(std::string message) {
    return {false, std::move(message)};
  }
};

}  // namespace workflow
