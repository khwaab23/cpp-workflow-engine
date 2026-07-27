#pragma once

#include <mutex>

#include "workflow/workflow_result.hpp"

namespace workflow {

class MetricsCollector {
 public:
  void recordSuccess();
  void recordFailure();
  void recordSkipped();
  void recordRetry();
  [[nodiscard]] ExecutionMetrics snapshot() const;

 private:
  mutable std::mutex mutex_;
  ExecutionMetrics metrics_;
};

}  // namespace workflow
