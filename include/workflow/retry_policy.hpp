#pragma once

#include <chrono>
#include <cmath>
#include <cstddef>

namespace workflow {

struct RetryPolicy {
  std::size_t maxRetries{0};
  std::chrono::milliseconds initialBackoff{0};
  double backoffMultiplier{1.0};

  [[nodiscard]] std::chrono::milliseconds backoffForAttempt(
      std::size_t retryAttempt) const {
    const auto factor =
        std::pow(backoffMultiplier, static_cast<double>(retryAttempt - 1));
    return std::chrono::milliseconds{
        static_cast<std::chrono::milliseconds::rep>(
            static_cast<double>(initialBackoff.count()) * factor)};
  }
};

}  // namespace workflow
