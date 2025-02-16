An orphan process is a process whose parent has terminated or exited, but the child process is still running.

In simpler words:
- The parent process ends or dies before the child process.
- As a result, the child process becomes an orphan.
- The child process still continues running (it doesn’t stop just because the parent dies).

What happens to an orphan process?
- In Linux, the init process (PID 1) takes over and becomes the new parent for the orphaned child.
- The init process ensures the orphan process gets cleaned up properly later on (if needed).

A zombie process is a process that has finished executing, but it still exists in the process table because its parent hasn’t read its exit status yet.

In simpler words:
- The process has completed its execution, but its entry in the process table is still there because the parent hasn’t used `wait()` to fetch the exit status of the child.
- It’s like a dead process that hasn’t been fully "cleaned up" yet, so it still takes up space in the process table.

What causes a zombie process?
- The child process finishes execution but does not completely disappear until the parent process calls `wait()` to read its exit status.
- If the parent doesn't call `wait()` to collect the exit status, the child process stays in the system as a zombie.
