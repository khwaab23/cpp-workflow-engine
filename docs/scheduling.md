# Scheduling

The FIFO scheduler initializes one remaining-dependency counter per task.
Root tasks enter the ready queue immediately. A successful task decrements each
direct dependent exactly once; a dependent becomes ready when its counter
reaches zero. This makes scheduling O(V + E), avoiding repeated full-graph
scans.

On failure, transitive dependents are marked skipped. Unrelated branches
continue. A restored succeeded task satisfies its dependents without being
executed again.
