#include "workflow/observability/logger.hpp"

#include <ostream>

namespace workflow {

Logger::Logger(std::ostream& output) : output_(output) {}

void Logger::log(LogLevel level, std::string_view message) {
  const char* label = "INFO";
  if (level == LogLevel::Warning) {
    label = "WARN";
  } else if (level == LogLevel::Error) {
    label = "ERROR";
  }
  std::lock_guard lock(mutex_);
  output_ << '[' << label << "] " << message << '\n';
}

}  // namespace workflow
