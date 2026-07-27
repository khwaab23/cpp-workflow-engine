# Persistence

`IStateStore` separates checkpoint policy from storage. `FileStateStore` writes
a versioned, quoted text format to a temporary file and renames it into place.
It rejects workflow identifiers that could escape the configured directory.

The engine checkpoints after every completed task. On restart it restores
succeeded tasks and runs pending, failed, or previously skipped tasks again.
This provides at-least-once behavior: task functions should be idempotent when
recovery matters.
