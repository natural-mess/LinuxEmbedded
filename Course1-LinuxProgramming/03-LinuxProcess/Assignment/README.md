# Linux Process Exercises

## Exercise 1. Initialize and Track a Process
**Description:** Write a C program that creates a child process using `fork()`. After creating the process, the child process will print its PID, and the parent process will print the PID of the child process.

To run the program:
```
cd Ex1
```
```
make
```
```
./createProcess
```

---

## Exercise 2. Use `exec` and Environment Variables
**Description:** Write a program in which the child process replaces the currently running program with another program, such as `ls` or `date`, using `execlp()` or `execvp()`.  
**Requirements:**  
- Pass environment variables into the program and perform different tasks (e.g., if the environment variable passed is 1, execute the `ls` command; if it is 2, execute the `date` command).  
- Explain what happens to the original process after `exec` is called.

When exec() is called:
- The current process image is completely replaced.
- A new program is loaded into memory (new code, new data, new heap, new stack).
- The PID remains the same, but the old program's memory layout is gone.

To run the program:
```
cd Ex2
```
```
make
```
```
./exec
```

---

## Exercise 3. Process Signals
**Description:** Write a program that creates two processes: a parent and a child. The parent process will send the `SIGUSR1` signal to the child process after a certain amount of time.  
**Requirements:**  
- Use `signal()` to define the behavior of the child process when it receives the `SIGUSR1` signal.  
- Print a message when the child process receives the signal.

To run the program:
```
cd Ex3
```
```
make
```
```
./signal
```

---

## Exercise 4. Record Process Status
**Description:** Write a program that creates a child process, after which the parent process uses `wait()` to wait for the child process to terminate and record its exit status.  
**Requirements:**  
- In the child process, choose the exit status by calling `exit()` with different values.  
- Use `WIFEXITED()` and `WEXITSTATUS()` to verify and record the exit code of the child process.

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

```
cd Ex4
```
```
make
```
```
./processStatus
```

---

## Exercise 5. Create Zombie and Orphan Processes
**Description:** Write a program that creates a zombie process and an orphan process to understand these special types of processes.  
**Requirements:**  
- To create a zombie, have the parent process terminate early while the child process is still running, and use `ps` to observe the zombie state of the process.  
- To create an orphan, have the child process sleep for a longer duration than the parent process, causing the child to become an orphan.  
- Explain the zombie and orphan states, along with the reasons why they occur in Linux.

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


```
cd Ex5
```
```
make
```
```
./zombieOrphan
```
