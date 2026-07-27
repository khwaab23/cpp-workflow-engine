# Design decisions

| Problem | Decision and why | Alternative and tradeoff |
|---|---|---|
| Invalid topology | Require a DAG so readiness and terminal state are deterministic. | Cycles require different execution semantics. |
| Concurrency bound | Use an RAII thread pool to cap resource use. | One thread per task is simpler but unbounded. |
| Readiness cost | Maintain dependency counters for O(V + E) scheduling. | Repeated scans use less state but do unnecessary work. |
| Idle workers | Use a condition variable with predicates. | Polling wastes CPU and adds latency. |
| Component boundaries | Compose focused concrete classes. | Deep inheritance obscures ownership and behavior. |
| Persistence | Put `FileStateStore` behind `IStateStore`. | Direct file coupling prevents deterministic tests and future stores. |
| Concurrent graph edits | Freeze topology during a run. | Dynamic edits greatly complicate synchronization. |
| Expected failure | Return `TaskResult`. | Exceptions blur domain failure and infrastructure failure. |
| Ownership | Use values and `unique_ptr` by default. | Broad shared ownership makes lifetime unclear. |
| Deployment scope | Implement a single-process engine honestly. | A nominal distributed abstraction without leases or consensus is unsafe. |

The engine deliberately exposes no scheduling strategy interface yet. FIFO is
the only required policy; an interface should appear only when a second real
policy exists.
