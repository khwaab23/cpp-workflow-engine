#include "workflow/observability/metrics.hpp"

namespace workflow {

void MetricsCollector::recordSuccess() {
  std::lock_guard lock(mutex_);
  ++metrics_.succeeded;
}
void MetricsCollector::recordFailure() {
  std::lock_guard lock(mutex_);
  ++metrics_.failed;
}
void MetricsCollector::recordSkipped() {
  std::lock_guard lock(mutex_);
  ++metrics_.skipped;
}
void MetricsCollector::recordRetry() {
  std::lock_guard lock(mutex_);
  ++metrics_.retries;
}
ExecutionMetrics MetricsCollector::snapshot() const {
  std::lock_guard lock(mutex_);
  return metrics_;
}

}  // namespace workflow
