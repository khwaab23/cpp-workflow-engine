cmake_minimum_required(VERSION 3.20)
project(cpp_workflow_engine VERSION 1.0.0 LANGUAGES CXX)

option(ENABLE_SANITIZERS "Enable address and undefined behavior sanitizers" OFF)
option(BUILD_EXAMPLES "Build examples" ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CompilerWarnings)
include(Sanitizers)

add_library(workflow_engine
  src/task.cpp
  src/workflow_engine.cpp
  src/graph/workflow_graph.cpp
  src/graph/workflow_validator.cpp
  src/runtime/scheduler.cpp
  src/runtime/thread_pool.cpp
  src/storage/file_state_store.cpp
  src/observability/logger.cpp
  src/observability/metrics.cpp)
target_include_directories(workflow_engine PUBLIC include)
target_compile_features(workflow_engine PUBLIC cxx_std_20)
workflow_set_warnings(workflow_engine)
workflow_enable_sanitizers(workflow_engine)

if(BUILD_EXAMPLES)
  add_executable(workflow_demo examples/basic_workflow.cpp)
  target_link_libraries(workflow_demo PRIVATE workflow_engine)
  add_executable(workflow_parallel_demo examples/parallel_workflow.cpp)
  target_link_libraries(workflow_parallel_demo PRIVATE workflow_engine)
  add_executable(workflow_retry_demo examples/retry_workflow.cpp)
  target_link_libraries(workflow_retry_demo PRIVATE workflow_engine)
  add_executable(workflow_failure_demo examples/failure_workflow.cpp)
  target_link_libraries(workflow_failure_demo PRIVATE workflow_engine)
endif()

include(CTest)
if(BUILD_TESTING)
  add_executable(workflow_tests
    tests/test_main.cpp
    tests/unit/core_test.cpp
    tests/unit/state_store_test.cpp
    tests/integration/workflow_execution_test.cpp)
  target_link_libraries(workflow_tests PRIVATE workflow_engine)
  add_test(NAME workflow_tests COMMAND workflow_tests)
endif()

add_executable(workflow_benchmark benchmarks/workflow_benchmark.cpp)
target_link_libraries(workflow_benchmark PRIVATE workflow_engine)
