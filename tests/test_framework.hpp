#pragma once

#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace test {

using Test = std::pair<std::string, std::function<void()>>;
inline std::vector<Test>& registry() {
  static std::vector<Test> tests;
  return tests;
}
inline bool add(std::string name, std::function<void()> function) {
  registry().emplace_back(std::move(name), std::move(function));
  return true;
}
inline void require(bool condition, const char* expression) {
  if (!condition) {
    throw std::runtime_error(std::string{"requirement failed: "} + expression);
  }
}

}  // namespace test

#define TEST(name)                                      \
  static void name();                                   \
  static const bool name##_registered =                 \
      test::add(#name, name);                           \
  static void name()
#define REQUIRE(expression) test::require((expression), #expression)
