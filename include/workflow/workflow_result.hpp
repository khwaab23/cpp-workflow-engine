#pragma once

#include <chrono>
#include <cstddef>
#include <string>
#include <unordered_map>

#include "workflow/state/workflow_snapshot.hpp"

namespace workflow {

struct ExecutionMetrics {
  std::size_t succeeded{0};
  std::size_t failed{0};
  std::size_t skipped{0};
  std::size_t retries{0};
  std::chrono::milliseconds elapsed{0};
};

struct WorkflowResult {
  bool success{false};
  WorkflowSnapshot snapshot;
  ExecutionMetrics metrics;
};

}  // namespace workflow
