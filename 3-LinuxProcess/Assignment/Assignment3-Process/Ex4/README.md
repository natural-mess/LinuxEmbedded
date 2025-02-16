`exit(int status)` – Terminate a Process

`exit()` is used to terminate a process and return an exit status code to the parent process.

How it Works
- When a process calls `exit()`, it ends immediately.
- The integer argument (status) is returned to the parent process.
- The parent process can collect this status using `wait()` or `waitpid()`.

`WIFEXITED(status)` – Check if Process Terminated Normally
- This macro is used with `wait()` or `waitpid()` to check if a child process exited normally.
- It returns true (nonzero) if the child process exited normally using `exit()`.

`WEXITSTATUS(status)` – Get Exit Status Code
- Once `WIFEXITED(status)` confirms that the process exited normally, WEXITSTATUS(status) retrieves the actual exit code.
- The exit status code is the number given inside exit() in the child process.