# IPC Signal Exercises

## Exercise 1. Catch the SIGINT Signal
**Requirements:** Write a program that catches the `SIGINT` signal (sent when pressing Ctrl+C) and prints a message when the signal is received.  
- Use the `signal()` function to register a handler for the `SIGINT` signal.  
- In the handler function, print the message "SIGINT received" each time the `SIGINT` signal is received.  
- The program should continue running until it receives the `SIGINT` signal for the third time, then terminate.  
**Hint:** Use a global counter variable to track the number of times the `SIGINT` signal is received and check this variable in the handler function.  
**Question:** If the `SIGINT` signal is ignored, what happens when Ctrl+C is pressed?

If ignoring SIGINT, when pressing Ctrl+C, system will still send signal SIGINT to process, but process will not react to that signal. This means that process will keep running. In short, if ignoring SIGINT, nothing will happen if we press Ctrl+C.

To run the program:
```
cd Ex1
```
```
make
```
```
./sigint
```

---

## Exercise 2. Use SIGALRM Signal to Create a Timer
**Requirements:** Write a program that creates a timer using the `SIGALRM` signal.  
- Use the `alarm()` function to trigger `SIGALRM` every second.  
- When the `SIGALRM` signal is received, increment a counter variable and print the line "Timer: <seconds> seconds".  
- Stop the program after counting to 10 seconds.  
**Hint:** Use `signal()` to register a handler for the `SIGALRM` signal, and call `alarm(1)` to make the timer repeat every second.  
**Question:** What happens if `alarm(1)` is not called again in the handler function?

`SIGALRM` is a signal that is sent to a process when a timer set by the `alarm()` function expires.

`SIGALRM` is used to notify a process that a specified amount of time has passed. You can think of it as a built-in timer that, when it goes off, sends this signal to your process.

- A process calls `alarm(seconds)`, setting a timer.
- After the specified number of seconds, the kernel sends the `SIGALRM` signal to that process.
- If the process has a signal handler for `SIGALRM`, that handler will be invoked.
- If no handler is set, the default action is usually to terminate the process.

`pause()` is a system call that suspends the calling process until a signal is received. In simple terms, when you call `pause()`, your program stops executing and "sleeps" until any signal (that is not ignored) interrupts it. Once a signal is delivered and the corresponding signal handler (if set) is executed, `pause()` returns, and your process continues execution (or terminates if the signal caused it to do so).

The `pause()` system call makes your process block (i.e., wait) until any signal is delivered. 

When the program starts, no alarm is scheduled yet. So, the first `alarm(1);` ensures that the first `SIGALRM` arrives after 1 second.

After the first SIGALRM arrives, the signal handler runs. By calling `alarm(1);` inside the handler, we ensure that another `SIGALRM` will arrive after another 1 second.

If we don't set another `alarm(1);` inside the handler, then the alarm will never happen again.

To run the program:
```
cd Ex2
```
```
make
```
```
./sigalrm
```

---

## Exercise 3. Create Inter-Process Communication Using Signals
**Requirements:** Write a program that creates two processes (parent and child) and uses the `SIGUSR1` signal for communication between them.  
**Implementation:**  
- Use `fork()` to create a child process from the parent process.  
- The parent process sends the `SIGUSR1` signal to the child process every 2 seconds.  
- When the child process receives the `SIGUSR1` signal, it prints the message "Received signal from parent".  
- Stop the program after sending the signal 5 times.  
**Hint:** Use `kill()` to send the signal from the parent to the child, and use `signal()` in the child process to catch the `SIGUSR1` signal.  
**Question:** Why do we need to use `kill()` to send signals in this exercise?

In this code, the `kill()` function is used to send signals between the parent and child processes because it’s the standard POSIX system call designed for that purpose in Unix-like systems. The `kill()` function allows one process to send a signal to another process (or itself), identified by its process ID (PID). Signals are a fundamental mechanism in Unix-like systems for inter-process communication (IPC) and control. They can notify a process of an event (like `SIGUSR1` or `SIGUSR2` in your code) or terminate it (like `SIGKILL`).

After the `fork()`, the parent and child are separate processes with their own memory spaces. They can’t directly call each other’s functions or share variables (without additional mechanisms like shared memory). Signals, sent via `kill()`, are one of the simplest ways for them to talk to each other.

In Unix, `kill()` is the way—it’s baked into the system’s design. The function’s flexibility (sending any signal to any PID the caller has permission to signal) makes it ideal for this task. The only caveat is that the sender needs to know the target’s PID, which is why `fork()` is useful—it returns the child’s PID to the parent and allows the child to get the parent’s PID with `getppid()`.

To run the program:
```
cd Ex3
```
```
make
```
```
./processSignal
```

---

## Exercise 4. Write a Program to Ignore the SIGTSTP Signal
**Requirements:** Write a program that can ignore the `SIGTSTP` signal (typically triggered by Ctrl+Z).  
- Use `signal()` or `sigaction()` to catch and ignore the `SIGTSTP` signal.  
- When the `SIGTSTP` signal is received, the program should not stop but continue running and print the message "SIGTSTP ignored".  
**Hint:** Ensure the program continues running instead of being suspended when Ctrl+Z is pressed.  
**Question:** What happens if the `SIGTSTP` signal is not handled and the user presses Ctrl+Z?

This code overrides the default action of SIGTSTP by handling it with sigtstp_handler. This means:
- Instead of suspending the process, it prints "SIGTSTP ignored" whenever Ctrl+Z is pressed.
- The process keeps running instead of stopping.

If we remove the signal handler (`signal(SIGTSTP, sigtstp_handler);`), then:
- Pressing Ctrl+Z will suspend the process (default SIGTSTP behavior).
- The process stops running until resumed manually.
- We can check the stopped process by running jobs in the terminal.

To run the program:
```
cd Ex4
```
```
make
```
```
./ignoreSigtstp
```

---

## Exercise 5. Program Waiting for Signals While Allowing User Input
**Objective:** Write a program that can handle signals while simultaneously allowing user input from the keyboard.  
**Instructions:**  
- Use `select()` or `poll()` to enable the program to process signals without interrupting keyboard input.  
- When the `SIGINT` signal is received, print the message "SIGINT received."  
- If the `SIGTERM` signal is received, terminate the program.  
**Hint:** Combine signal handling with input reading to make the program more flexible.

`volatile`: Tells the computer, “Hey, these might change unexpectedly (by signals), so always check their real value.”
`sig_atomic_t`: A special type that’s safe to use with signals (it’s like a tiny, fast number that won’t get messed up if a signal interrupts things).

```c
struct pollfd fds[1];
fds[0].fd = STDIN_FILENO;
fds[0].events = POLLIN;
```
- This prepares the program to watch for keyboard input.
- `struct pollfd`: A little box (structure) that holds info about what we’re watching.
- `fds[1]`: An array (list) with 1 slot, called fds[0]. We’re only watching one thing here.

`fds[0].fd = STDIN_FILENO`:
- `.fd`: The “file descriptor” part of the box—what we’re watching.
- `STDIN_FILENO`: A number (usually 0) that means “keyboard input” (standard input, or “stdin”).
- So, this says, “Watch the keyboard.”

`fds[0].events = POLLIN`:
- .`events`: What we’re waiting for.
- `POLLIN`: A code that means “I want to know when there’s data to read (like typed text).”
- This tells the program to check for input from the keyboard.

```c
int ready = poll(fds, 1, 3000);  // wait for 3 seconds
```
- `poll()`: A function that waits and checks if something we’re watching (like the keyboard) is ready.
- `fds`: The list of things to watch (our `fds[0]` for keyboard input).
- `1`: How many things we’re watching (just 1 slot in fds).
- `3000`: How long to wait, in milliseconds (3000 ms = 3 seconds).
- `int ready`: Stores what `poll()` tells us:
    - `> 0`: Something’s ready (e.g., you typed something).
    - `0`: Nothing happened (timeout after 3 seconds).
    - `< 0`: An error occurred (rare here).
  
Think of `poll()` as a lookout: it watches the keyboard for 3 seconds, then reports back.

```c
if (ready > 0)
{
    if (fds[0].revents & POLLIN)
    {
```
- `if (ready > 0)`: If `poll()` says something’s ready…
- `fds[0].revents & POLLIN`:
    - `.revents`: The “results” part of our `fds[0]` box—what actually happened.
    - `& POLLIN`: A trick to check if “data is ready” (POLLIN) is one of the results.
    - If true, it means you typed something and pressed Enter.

```c
char input[100];
if (fgets(input, sizeof(input), stdin) != NULL)
{
    printf("User input: %s", input);
}
```
`fgets(input, sizeof(input), stdin)`:
- `fgets`: Grabs a line of text you typed.
- `input`: Where to store it.
- `sizeof(input)`: How big the bucket is (100 characters).
- `stdin`: Where to read from (keyboard input).
- Returns the text if it worked, or NULL if it didn’t (e.g., end of input).

To run the program:
```
cd Ex5
```
```
make
```
```
./signalFromUser
```
