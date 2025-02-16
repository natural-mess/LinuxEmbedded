# Lesson 3 - Linux process

## 1 - Introduction
Program is a group of instructions that perform a specific task, which can be performed by executable files and located on the computer's hard drive. 

Process is a program that is executing and using system resources. Once a program starts running, it becomes a process.

Process ID (PID) is a positive and unique integer for each process on the system. Every time a new process starts, the OS gives it a unique number (PID) to track it.

Command to view processes running in Linux system
```
ps -aux
```

Find process names app running in system
```
ps -aux | grep -iE <appName>
```

## 2 - Command-line argument
Shows all flags supported by `ls`
```
ls -l
```

```
ls -lh
```

```
ls --help
```

Main function with argument

```c
void main(int argc, char *argv[])
```

```argc```: number of arguments
```argv```: content of each argument

```argv[0]```: name of program

## 3 - Memory layout

![alt text](_assets/memoryLayout.png)

- Text segment stores the actual machine code (compiled instructions) of the running program. It's read-only (prevents accidental modifications). It's shared among processes running the same program to save memory.
- Initialized Data Segment: contains global variables and static variables that have been **explicitly**(tuờng minh) initialized. This segment has read and write permission.
- Uninitialized Data: contains global variables and static variables that have been **implicitly**(không tuờng minh) initialized. This segment has read and write permission.
- Heap Segment: this segment is for dynamic memory allocation (from malloc() / new). it grows upward (toward higher memory addresses). Memory must be freed manually (free() in C, delete in C++). If not managed properly, can cause memory leaks. The endpoint of heap is called a program break. This segment has read and write permission.
- Stack Segment: contains stack frames and local variables. Grows downward (toward lower memory addresses). Automatically managed (cleaned up after function calls). Too many nested function calls can cause Stack Overflow. This segment has read and write permission.

A stack frame is a section of the stack that gets created every time a function is called. It stores information about the function call, such as:
- Local variables
- Function arguments
- Return address (where to go after the function finishes)
- Saved registers (to restore previous state)
- Each function call gets its own stack frame, and when the function finishes, its stack frame is removed from the stack (LIFO – Last In, First Out).

How it works?
- A function is called → A new stack frame is pushed onto the stack.
- Function executes → Stores local variables, parameters, and return address.
- Function returns → The stack frame is popped (removed), restoring the previous function’s state.

Example: 
```c
#include <stdio.h>

void funcB() {
    int b = 20;  // Local variable (inside stack frame of funcB)
    printf("Inside funcB, b = %d\n", b);
}

void funcA() {
    int a = 10;  // Local variable (inside stack frame of funcA)
    funcB();     // Calls funcB (new stack frame created)
    printf("Inside funcA, a = %d\n", a);
}

int main() {
    funcA();  // Calls funcA (new stack frame created)
    return 0;
}
```

```
+----------------------+
| main() Stack Frame   |  <--- Created first
|  (Local variables)   |
+----------------------+
| funcA() Stack Frame  |  <--- Created when funcA() is called
|  int a = 10          |
+----------------------+
| funcB() Stack Frame  |  <--- Created when funcB() is called
|  int b = 20          |
+----------------------+
```

Valgrind: tool used to debug memory problem

Install Valgrind
```
sudo apt install valgrind  # Ubuntu, Debian, etc.
sudo yum install valgrind  # RHEL, CentOS, Fedora, etc.
sudo pacman -Syu valgrind  # Arch, Manjaro, Garuda, etc.
sudo pkg ins valgrind      # FreeBSD
```
Run Valgrind
```
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         --log-file=valgrind-out.txt \
         ./executable exampleParam1
```
The flags are, in short:
- `--leak-check=full`: "each individual leak will be shown in detail"
- `--show-leak-kinds=all`: Show all of "definite, indirect, possible, reachable" leak kinds in the "full" report.
- `--track-origins=yes`: Favor useful output over speed. This tracks the origins of uninitialized values, which could be very useful for memory errors. Consider turning off if Valgrind is unacceptably slow.
- `--verbose`: Can tell you about unusual behavior of your program. Repeat for more verbosity.
- `--log-file`: Write to a file. Useful when output exceeds terminal space.

If we have memory leak, the log will have the following information:
```
5 bytes in 1 blocks are definitely lost in loss record 1 of 1
   at 0x4C29BE3: malloc (vg_replace_malloc.c:299)
   by 0x40053E: main (in /home/Peri461/Documents/executable)
```

**Reference**: https://stackoverflow.com/questions/5134891/how-do-i-use-valgrind-to-find-memory-leaks

## 4 - Operations on Process

![alt text](_assets/process.png)

- Use System Call fork() to create a new process
- The process calling fork() is called the parent process
- New process are created by parent process is called child process
- Init process is the first process that is run, is the parent of all other processes and has a pid of 1

Example:
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char const *argv[])   /* Cấp phát stack frame cho hàm main() */
{
    /* code */
    pid_t child_pid;                /* Lưu trong stack frame của main() */
    int counter = 2;                /* Lưu trong frame của main() */

    printf("Gia tri khoi tao cua counter: %d\n", counter);

    child_pid = fork();         
    if (child_pid >= 0) {
        if (0 == child_pid) {       /* Process con */
            printf("\nIm the child process, counter: %d\n", ++counter);
            printf("My PID is: %d, my parent PID is: %d\n", getpid(), getppid());
            
        } else {                    /* Process cha */
            printf("\nIm the parent process, counter: %d\n", ++counter);
            printf("My PID is: %d\n", getpid());
	    while (1);
        }
    } else {
        printf("fork() unsuccessfully\n");      // fork() return -1 nếu lỗi.
    }

    return 0;
}
```

When a process calls `fork()`, it creates a new child process that is a duplicate of the parent. After `fork()`, both processes run independently but share the same code.

When `fork()` is called, both the parent and child processes continue execution from the same point in the code—but they do not restart from the beginning of main().

Each process only executes the code that follows `fork()` in its own execution branch.

The OS usually schedules the parent first because it was already running before fork().

However, this is not guaranteed—sometimes the child might execute first, depending on the OS scheduler.

**Step-by-Step Execution of fork()**

1. The parent starts executing main():
```c
child_pid = fork();
```
A new child process is created.
- In the parent, fork() returns the child’s PID (e.g., 1235).
- In the child, fork() returns 0.
2. Two independent processes now exist:
- Parent process (PID = 1234)
    - child_pid = 1235 (child’s PID).
    - It enters the else block (child_pid > 0).
- Child process (PID = 1235)
    - child_pid = 0 (since fork() returns 0 to the child).
    - It enters the if block (child_pid == 0).

When you call fork(), the operating system (OS) creates a new process by duplicating the parent process.
- Parent process (before fork() call)
    - Calls fork().
    - The OS creates a child process that is an exact copy of the parent.
    - The OS assigns a new Process ID (PID) to the child.
- Return values of fork():
    - In the parent process: fork() returns the child’s PID (a positive integer).
    - In the child process: fork() returns 0.

The OS ensures:
- The parent process gets the child’s PID (so it can track its child).
- The child process gets 0 (because it is now its own process).

### Exec Family
In Linux, the exec family of functions is used to replace the current process image with a new process image. These functions are commonly used in conjunction with fork() to create new processes.

- `execl`, `execlp`, `execle`: Arguments are passed as a list (variable arguments).
- `execv`, `execvp`, `execve`: Arguments are passed as an array (vector).
- Functions with `p` (e.g., `execlp`, `execvp`) search for the executable in the PATH environment variable.
- Functions without `p` require the full path to the executable.
- `execle` and `execve` allow you to specify a custom environment for the new process.

|Function|	Argument Format|	PATH Search|	Environment Handling	|Description|
|--------|-------------|--------------|--------------|--------------|
|execl|	List|	No|	Inherits current env|	Takes a list of arguments; requires the full path to the executable.|
|execlp|	List|	Yes|	Inherits current env|	Takes a list of arguments; searches for the executable in PATH.|
|execle|	List|	No|	Custom env	Takes a list of arguments; requires the full path; allows custom environment.|
|execv|	Array|	No|	Inherits current env	|Takes an array of arguments; requires the full path to the executable.
|execvp|	Array|	Yes|	Inherits current env|	Takes an array of arguments; searches for the executable in PATH.|
|execve|	Array|	No|	Custom env	|Takes an array of arguments; requires the full path; allows custom environment.|

The terms "Inherits current env" and "Custom env" refer to how environment variables are handled when a new process is created using these functions.
- Inherits Current Environment: The new process inherits the environment variables of the calling process (the parent process). This means the new process will have access to the same environment variables as the parent process.
- Custom Environment: The new process uses a custom set of environment variables provided by the programmer. The parent process's environment variables are not inherited unless explicitly included in the custom environment.

Finds location of ```ls``` command
```
which ls
```

Example: 
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) 
{    
    printf("Run command <ls -lah> after 2 seconds\n");
    sleep(2);
    
    execl("/bin/ls", "ls", "-l", "-a", "-h", NULL); 

    while(1);  // This line is NEVER executed if execl succeeds

    return 0;   
}
```

When you call `execl()`, it replaces the current process with a new program (`/bin/ls` in this case). If `execl` succeeds, the original program (your C program) ceases to exist, and control is never returned to `main()`. The current process's memory is overwritten by the new program (`ls`).

A process image is the entire memory layout of a running process. It includes:
- Code (Text) Segment – The program’s executable instructions.
- Data Segment – Global and static variables.
- Heap – Dynamically allocated memory (via malloc, etc.).
- Stack – Function calls, local variables, return addresses, etc.
- File Descriptors – Open files, sockets, etc.

### Process Termination
- **Normally termination**: A process can complete its execution by calling the system call _exit() or using exit() function. In C, when main() finishes, it automatically calls exit(0) internally.
- A**bnormally termination**: A process can be killed by using system call kill() or kill command in Linux.

Process image: all memory space of process

## 5 - Process Management

![alt text](_assets/processManagement.png)

System call wait(): Blocks parent until a child exits.
System call waitpid(): More advanced, can wait for specific children.

**Question**

What will happen if we call fork() in loop ? Child process will copy data in the loop or outside of the loop ?

When fork() is called inside a loop, the child process inherits the entire state of the parent, including variables, program counter, and loop state at the time of fork().

The child copies everything from the parent at the moment fork() is executed, meaning:
- All variables (including loop counters) are copied.
- The child starts executing from the next line after fork().
- The loop continues independently in both parent and child.

Example:
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    for (int i = 0; i < 3; i++) { 
        pid_t pid = fork(); 
        
        if (pid == 0) {  // Child process
            printf("Child Process (PID: %d, Parent PID: %d), i=%d\n", getpid(), getppid(), i);
            exit(0); // Exit so that child doesn't continue looping
        } else if (pid > 0) {  // Parent process
            printf("Parent Process (PID: %d), i=%d\n", getpid(), i);
        } else {
            perror("fork failed");
            exit(1);
        }
    }
    sleep(1); // Allow processes to run before termination
    return 0;
}
```

Output:
```yaml
Parent Process (PID: 1000), i=0
Child Process (PID: 1001, Parent PID: 1000), i=0
Parent Process (PID: 1000), i=1
Child Process (PID: 1002, Parent PID: 1000), i=1
Parent Process (PID: 1000), i=2
Child Process (PID: 1003, Parent PID: 1000), i=2
```

- The parent process continues looping.
- Each time fork() is called, a child process is created.
- The child copies all variables, including the current value of i.
- The child process does NOT inherit future loop iterations.
- The child starts execution after fork(), so it does not repeat the loop.
- That’s why we use exit(0); inside the child to prevent it from executing the loop again. By calling exit(0) in the child process, the child terminates after its initial work, and it does not continue to call fork() again, thus preventing it from creating additional processes.

## 6 - Orphane and Zombie Process
The parent process terminates before the child process, so who will get the child process's termination state ?

An orphan process is a process whose parent has terminated or exited, but the child process is still running.

In simpler words:
- The parent process ends or dies before the child process.
- As a result, the child process becomes an orphan.
- The child process still continues running (it doesn’t stop just because the parent dies).

What happens to an orphan process?
- In Linux, the init process (PID 1) takes over and becomes the new parent for the orphaned child.
- The init process ensures the orphan process gets cleaned up properly later on (if needed).

Example:
- Parent Process (A) starts child process (B).
- Parent Process (A) dies or ends before child process (B).
- Child Process (B) is still running and becomes an orphan.
- The init process (PID 1) adopts the orphaned process (B) as its new parent.

What if the child process terminates before the parent process can call system `wait()`?

A zombie process is a process that has finished executing, but it still exists in the process table because its parent hasn’t read its exit status yet.

In simpler words:
- The process has completed its execution, but its entry in the process table is still there because the parent hasn’t used `wait()` to fetch the exit status of the child.
- It’s like a dead process that hasn’t been fully "cleaned up" yet, so it still takes up space in the process table.

What causes a zombie process?
- The child process finishes execution but does not completely disappear until the parent process calls `wait()` to read its exit status.
- If the parent doesn't call `wait()` to collect the exit status, the child process stays in the system as a zombie.

Example:
- Parent Process (A) starts child process (B).
- Child Process (B) finishes executing, but its exit status hasn’t been read by the parent.
- Child Process (B) becomes a zombie process.
- The zombie process stays in the system until the parent calls wait().

Example of safe-fork:
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void func(int signum)
{
    printf("Im in func()\n");
    wait(NULL);
}

int main()
{
    pid_t child_pid = fork();
    
    if (child_pid >= 0) {
        if (child_pid == 0) {
            printf("I am Child, myPID: %d\n", getpid());
           // while(1);
        } else {
            /**When a child is terminated, a corresponding SIGCHLD signal 
             * is delivered to the parent
            */
            signal(SIGCHLD, func);
            printf("I am Parent\n");
            while(1);
        }
    } else {
        printf("fork() unsuccessfully\n");
    }
}
```
