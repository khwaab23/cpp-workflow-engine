# Failure model

Expected task failure is represented by `TaskResult`; thrown task exceptions
are caught at the task boundary and converted to failures. A retry policy
controls retry count and exponential backoff. Exhausted retries mark the task
failed and all transitive dependents skipped.

Invalid definitions, invalid configuration, corrupt checkpoints, and filesystem
errors are infrastructure failures and throw exceptions. They abort the run
because continuing cannot be proven safe.
