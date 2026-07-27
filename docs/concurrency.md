# Concurrency

`ThreadPool` owns a fixed number of workers, one protected job queue, and one
condition variable. Its destructor sets the stop flag, wakes all workers, and
joins them. Worker waits always use a predicate.

The graph is read-only during a run. Scheduler and task state transitions are
coordinated by the engine thread. Task functions run concurrently. The logger,
metrics collector, and file state store synchronize their own mutable data.
Task callables are responsible for synchronizing external objects they capture.
