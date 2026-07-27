# C++ Workflow Engine

A small C++20 workflow runtime for executing validated directed acyclic graphs
(DAGs) on a bounded thread pool.

## Features

- Parallel execution of independent tasks
- Explicit dependencies and cycle validation
- Configurable exponential retry policies
- Failure propagation to transitive dependents
- Atomic file checkpoints and restart from succeeded tasks
- Thread-safe logging and metrics
- Dependency-injected persistence
- No third-party runtime or test dependencies

## Build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Use `-DENABLE_SANITIZERS=ON` with GCC or Clang for AddressSanitizer and
UndefinedBehaviorSanitizer. Run `./build/workflow_demo` for a minimal example.

## API sketch

```cpp
workflow::WorkflowDefinition definition("daily-import");
definition.addTask(workflow::Task(
    "download", "Download", {},
    [] { return workflow::TaskResult::succeeded(); }));
definition.addTask(workflow::Task(
    "index", "Index", {"download"},
    [] { return workflow::TaskResult::succeeded(); }));

workflow::WorkflowEngine engine(
    std::move(definition),
    std::make_unique<workflow::FileStateStore>(".state"), 4);
auto result = engine.execute();
```

The graph is immutable during execution. Expected task failures are values;
invalid definitions and infrastructure failures are exceptions.

See [architecture](docs/architecture.md) and
[design decisions](docs/design-decisions.md) for the boundaries and tradeoffs.
