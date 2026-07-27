#pragma once

#include <iosfwd>
#include <mutex>
#include <string_view>

namespace workflow {

enum class LogLevel { Info, Warning, Error };

class Logger {
 public:
  explicit Logger(std::ostream& output);
  void log(LogLevel level, std::string_view message);

 private:
  std::ostream& output_;
  std::mutex mutex_;
};

}  // namespace workflow
