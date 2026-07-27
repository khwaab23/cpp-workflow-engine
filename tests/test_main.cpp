#include "test_framework.hpp"

int main() {
  int failures = 0;
  for (const auto& [name, run] : test::registry()) {
    try {
      run();
      std::cout << "[PASS] " << name << '\n';
    } catch (const std::exception& exception) {
      ++failures;
      std::cerr << "[FAIL] " << name << ": " << exception.what() << '\n';
    }
  }
  return failures == 0 ? 0 : 1;
}
