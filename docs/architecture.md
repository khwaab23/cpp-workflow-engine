# Architecture

`WorkflowDefinition` owns task metadata and behavior. `WorkflowValidator`
rejects missing dependencies, duplicates, self-dependencies, and cycles before
execution. `WorkflowGraph` builds the immutable reverse dependency index.

`WorkflowEngine` is the composition root and coordinator. It delegates readiness
to `Scheduler`, work ownership to `ThreadPool`, persistence to `IStateStore`,
and counters to `MetricsCollector`. It does not own graph algorithms, worker
loops, or serialization.

```text
WorkflowDefinition -> WorkflowValidator -> WorkflowEngine
                                             |  |  |
                                  Scheduler -+  |  +- IStateStore
                                  ThreadPool ---+       |
                                                    FileStateStore
```

Public headers live under `include/workflow`; implementations live under `src`.
The definition/graph remains immutable while a separate `WorkflowSnapshot`
tracks execution state, attempts, and errors.
