# Lesson 5 - IPC Signal

## 1 - Introduction
Signals are a software interrupt and one of the oldest methods for inter-process communication (IPC) in Unix systems. They handle asynchronous events, which can originate externally (e.g., pressing Ctrl+C) or from program operations (e.g., division by zero).

Signal Lifecycle: A signal goes through 3 phases:
- Generation: The signal is raised/sent/generated.
- Delivery: The signal is pending until it is delivered to a process.
- Processing: The process handles the signal in one of three ways:
  - Ignore the Signal:
    - No action is taken
    - Except SIGKILL and SIGSTOP cannot be ignored.
  - Catch and Handle the Signal:
    - The kernel suspends the main thread and jumps to a user-registered signal handler.
    - SIGINT and SIGTERM are frequently used.
    - SIGKILL and SIGSTOP cannot be caught.
  - Perform the Default Action:
    - This depends on the specific signal.

**Processing**
1. First, main program begins
2. A signal is created through system call signal function
3. Program execution continues
4. When signal is sent to main function, immediately, main function will call Signal Handler which is registered in step 2.
5. Main process will stop and Signal Handler will execute because Signal Handler has higher priority than normal process (it's a software interrupt).
6. Signal Handler returns
7. Program execution continues

We say Signal handler is sentitive in time because when signal handler is running, other threads or other processes have to wait. So if signal handler has any problem related to delay or sleep, or handler is stuck, then other processes will be blocked forever. So we should exit Signal Handler as soon as possible. Only important job should be in Signal Handler.

## 2 - Signal Handler
- Signal handling is registered using the signal() system call.
- Signal handlers are time-sensitive because they interrupt the process's normal execution. Therefore, signal handlers should execute and exit as quickly as possible.
```c
sighandler_t signal (int signo, sighandler_t handler);
```
The signal() function takes two arguments: the signal number (signo) and a pointer to the signal handler function (handler).

To list all signal supported by our device and signal number
```bash
kill -l
```

## 3 - Common Signals
- SIGKILL: Terminates the process; cannot be caught or ignored. Sent with the kill() system call.
- SIGTERM: Intended for graceful termination; the process can catch this signal and perform cleanup before exiting. Sent with the kill() system call.
- SIGINT: Sent to foreground processes (typically from Ctrl+C); default action is to terminate the process.
- SIGCHLD: Sent to a parent process when a child process stops or terminates; it's ignored by default.
- SIGSTOP: Suspends the process; cannot be caught or ignored. Sent with the kill() system call.
- SIGUSR1/SIGUSR2: User-defined signals.

Uncatchable Signals: SIGKILL and SIGSTOP cannot be caught or ignored, ensuring that there's always a way to terminate or suspend a process.
- It's designed as a low-level signal to help the system manage processes. Allowing processes to catch or ignore SIGSTOP would break the ability of the operating system to manage processes reliably. If a process could ignore or catch SIGSTOP, it might prevent itself from being paused, which could lead to undesirable behavior, like never allowing debugging or the process being paused by the operating system.
- The SIGKILL signal is meant to be the ultimate way to stop a process, regardless of its state. If a process could catch or ignore SIGKILL, it could potentially never be terminated, which would defeat the purpose of this signal. The signal is designed to be a guaranteed way to terminate a process, even if it's stuck or in a "bad" state.

Ctrl+C: Pressing Ctrl+C sends SIGINT (signal number 2) to the foreground process, which typically terminates it.

- When running a program using `./app`, we are running it in foreground, in this case, we cannot run any other commands in Terminal.
- To run program in background, we can use `./app &`. Terminal will print pid of process and release the Terminal for us to run other commands.

kill Command: The kill command can be used to send specific signals to processes, e.g., `kill -9 <pid>` sends SIGKILL.

To pause a process:
```
kill -19 <pid>
```

To resume the process:
```
kill -18 <pid>
```

Find source code version of kernel:
```
uname -r
```
Search in website Elixir Bootlin the output of above command.

## 4 - Sending Signals
- Signals can be sent using the kill() system call in code or via the kill command in the terminal.
- A process can send a signal to itself using getpid().
```c
int kill (pid_t pid, int signo);
```
The kill() function takes the process ID (pid) and the signal number (signo) as arguments.

## 5 - Blocking and Unblocking Signals
- Blocking signals prevents them from interrupting a critical section of code.
- Each process can specify which particular signal it wants to block. When a signal is blocked, it doesn't disappear, it will be held by the kernel in a pending queue for processing.
- Blocked signals are held pending by the kernel until they are unblocked.
- The list of blocked signals is called the `signal mask`.

Signal sets:
```c
int sigemptyset (sigset_t *set);
int sigfillset (sigset_t *set);
int sigaddset (sigset_t *set, int signo);
int sigdelset (sigset_t *set, int signo);
int sigismember (const sigset_t *set, int signo);
```
In Linux, signals are asynchronous notifications sent to processes. Sometimes you don’t want a signal to interrupt your process immediately—instead, you might want to delay its handling. This is where blocking (and later unblocking) signals comes in, using a signal mask.

- Blocking a signal means telling the kernel, “Do not deliver this signal to me right away.” When a signal is blocked, if it occurs (i.e., if some event triggers that signal), it is not delivered immediately to the process. Instead, it is marked as pending. Once the process unblocks the signal, the kernel will then deliver any pending signals.
- Unblocking a signal removes it from the process’s block list, allowing the kernel to deliver it. Once unblocked, if that signal was pending (or if it happens afterward), it will be delivered and the process’s signal handler (if set) will be executed.

A signal mask is a set (or list) of signals that a process (or thread) has currently blocked. It allows a process to specify which signals it wants to temporarily ignore (block) while performing critical operations.

You can use functions like `sigprocmask()` to add signals to your mask (block them) or remove signals from your mask (unblock them).
```c
#include <signal.h>

sigset_t set;
sigemptyset(&set);             // Start with an empty set
sigaddset(&set, SIGTERM);      // Add SIGTERM to the set
sigprocmask(SIG_BLOCK, &set, NULL);  // Block SIGTERM
```

Similarly, you can remove signals from the mask.
```c
sigprocmask(SIG_UNBLOCK, &set, NULL);  // Unblock SIGTERM
```

`sigfillset()` initializes a signal set by adding all signals to it. It is typically used when you want to block all signals or start with a complete set of signals and then remove those you don't want.
```c
#include <signal.h>
#include <stdio.h>

int main() {
    sigset_t set;
    
    // Fill the set with all signals.
    if (sigfillset(&set) == -1) {
        perror("sigfillset");
        return 1;
    }
    
    // Now, 'set' contains every possible signal.
    printf("Signal set filled with all signals.\n");
    return 0;
}
```

`sigdelset()` removes a specified signal from a signal set. After filling a signal set with all signals (using sigfillset()) or after initializing it in another way, you might want to exclude one or more signals from being blocked or handled. sigdelset() lets you do that.
```c
#include <signal.h>
#include <stdio.h>

int main() {
    sigset_t set;
    
    // First, fill the set with all signals.
    if (sigfillset(&set) == -1) {
        perror("sigfillset");
        return 1;
    }
    
    // Remove SIGTERM from the set.
    if (sigdelset(&set, SIGTERM) == -1) {
        perror("sigdelset");
        return 1;
    }
    
    // Now, 'set' contains all signals except SIGTERM.
    printf("SIGTERM removed from the signal set.\n");
    return 0;
}
```

When to Use Signal Masks
- Critical Sections: While executing a critical part of your code (e.g., modifying shared data), you might block certain signals to prevent them from interrupting your code.
- Temporary Suspension: A process can block signals temporarily, perform its task without interruptions, then unblock the signals to handle any that were queued.

Example:
```c
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    sigset_t set;
    
    // Start with a set containing all signals.
    if (sigfillset(&set) == -1) {
        perror("sigfillset");
        exit(EXIT_FAILURE);
    }
    
    // Suppose we don't want to block SIGINT (Ctrl+C), remove it from the set.
    if (sigdelset(&set, SIGINT) == -1) {
        perror("sigdelset");
        exit(EXIT_FAILURE);
    }
    
    // Now block all signals in 'set' (i.e., all signals except SIGINT).
    if (sigprocmask(SIG_BLOCK, &set, NULL) == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }
    
    printf("All signals except SIGINT are blocked.\n");
    
    // ... do critical work here ...
    
    return 0;
}
```

When block 2 signals SIGINT and SIGTERM, these signals will be pushed in pending signal queue. When unblock these 2 signals, SIGINT will be handled first, then SIGTERM will be handled because SIGINT has lower priority number (number 2) meaning higher priority than SIGTERM.
