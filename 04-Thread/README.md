# Lesson 4 - Thread

## 1- Introduction

A process is a program that is executing and using system resources. (A process is like a complete cooking project happening in the kitchen. It’s a program that’s running—like “making a cake.”)

Each process has its own space (like its own counter, oven, and ingredients). It doesn’t share these with other processes unless told to. For example, if you’re running a web browser and a music player at the same time, those are two separate processes. The browser doesn’t mess with the music player’s stuff.

- It’s independent: One process crashing (like burning the cake) doesn’t ruin another process (the soup still cooks fine).
- It has its own memory and resources, managed by the operating system (the kitchen boss).

*Real-world analogy: Imagine two chefs in the kitchen, each working on their own dish with their own tools. They don’t bump into each other unless they choose to share something.*

A thread is a lightweight process that can be managed independently by a scheduler. (A thread is like a single step or worker inside one of those cooking projects (a process). It’s a smaller piece of work that shares the same space and resources as the process it belongs to.)

Threads live inside a process. So, in our “making a cake” process, one thread might be mixing the batter, while another thread is preheating the oven.

They share the same memory and tools (like the same mixing bowl and oven), which makes them faster and lighter than starting a whole new process.

Example: In a web browser (a process), one thread might load a webpage while another thread plays a video on that page.

- Threads are faster to create and switch between because they don’t need their own separate space.
- But if one thread messes up (like spilling batter everywhere), it can mess up the whole process (the cake is ruined).

*Real-world analogy: One chef with two hands—one hand is stirring, the other is pouring. They’re part of the same job and share the same counter.*

Similar to processes, threads are created for the purpose of processing multiple tasks at the same time (multi-task)

A process always has a main process that is `main()` function.

In the Linux kernel, every thread is treated as a “task” with its own unique ID (called a TID, or thread ID). The scheduler manages these tasks individually, not the process as a group.

Example: A process might have three threads—Thread A (mixing batter), Thread B (preheating the oven), and Thread C (setting the table). The scheduler sees them as three separate tasks on its list.

The scheduler keeps a run queue (its to-do list) of all tasks ready to use the CPU. This includes threads from all processes. So, Thread A from Process 1 might be next to Thread X from Process 2 on the list.

Switching between threads in the same process is faster than switching between processes:
- They share memory, so the scheduler doesn’t need to swap out the whole “kitchen counter” (memory space)—it just changes which “hand” (thread) is working.
- It saves the current thread’s state (like where it is in the recipe) and loads the next thread’s state.

Since threads share the process’s resources (memory, files, etc.), the scheduler doesn’t separate them out—it just runs them as individual tasks. This makes thread scheduling lightweight but means they need to cooperate (or they’ll trip over each other).

### Thread Executing

On a multi-core syste, multiple threads can work in parallel.

In the same process, if one thread is blocked, the other threads will still work normally. A thread is “blocked” when it’s waiting for something—like input, a timer, or a resource (e.g., a mutex lock). It’s not crashed; it’s just paused. n Linux, the scheduler treats each thread as a separate task. If one thread is blocked, the scheduler skips it and gives CPU time to other ready threads in the same process (or others). Blocking only stops that thread’s work—it doesn’t inherently mess up the shared memory or resources, so the process keeps going.

In the same process, if one thread crashes badly enough, it often takes down the entire process—including all its threads.All threads in a process share the same memory and resources (like one chef’s counter in the kitchen). If one thread messes up that shared space, it affects everyone. A “crash” here means something serious—like a segmentation fault (accessing memory it shouldn’t), divide-by-zero, or another fatal error. When a thread crashes, the operating system (Linux kernel) detects the error. It doesn’t just stop that one thread—it terminates the whole process because the shared resources might now be corrupt or unstable. This sends a signal (like SIGSEGV for a segfault) to the process, and unless the program catches it (which is rare), the process—and all its threads—dies.

Every time a thread is created, they are placed in stack segments.

![alt text](_assets/fourThreads.png)

### Context Switching

When a process is removed from access to the processor, sufficient information on its current operating state must be stored. Because when it is again scheduled to run on the processor it can resume its operation from an identical position.

This operational state date is known as its context. The context of a process includes its address space, stack space, virtual address space, register set image (e.g. Program Counter (PC), Stack Pointer (SP)).

The act of removing the process of execution from the processor (and replacing it with another) is known as a process switch or context switch.

![alt text](_assets/contextSwitch.png)

A context switch is when the operating system (like Embedded Linux) pauses one task (a process or thread) that’s using the CPU and switches to another task. It’s how the system juggles multiple jobs on a single CPU, making it look like they’re running at the same time—even though only one can cook at once.

How context switch work:

- Step 1: Trigger the Switch
    - Something tells the scheduler it’s time to switch tasks.
    - This could be because:
        - The current task’s time is up (e.g., it’s run for 10 milliseconds).
        - The task is waiting for something (like sensor data) and can’t continue.
        - A more important task (higher priority) is ready.
    
Example: Thread 1 has been checking a temperature sensor for 10ms, and its time slice ends.

- Step 2: Pause the Current Task
    - The scheduler stops the current task from running.
    - The CPU gets an interrupt (a signal) from a timer or event, giving control to the scheduler.
    - The task is paused mid-work; the CPU stops executing its instructions.

Example: Thread 1 is paused while calculating the temperature.

- Step 3: Save the Current Task’s Context

    - The scheduler saves everything the CPU was doing for the current task so it can resume later.
    - All the details needed to restart, including:
        - Program Counter: Where in the code it was (e.g., “line 42”).
        - Registers: Temporary data in the CPU (e.g., numbers it was using).
        - Stack Pointer: Where its memory “scratchpad” was pointing.
    - This info is copied from the CPU into a memory area for that task.

Example: Thread 1’s state (e.g., “at line 42, register A = 72°F”) is saved.

- Step 4: Choose the Next Task
    - The scheduler picks the next task to run.
    - It checks its list (the run queue) of tasks that are ready.
    - Decision:
        - For normal tasks, it picks the one that’s waited longest (fairness, using the Completely Fair Scheduler).
        - For real-time tasks, it picks the highest-priority one.

Example: Thread 2, which updates the display, hasn’t run recently, so it’s chosen.

- Step 5: Load the New Task’s Context
    - The scheduler loads the saved details of the new task into the CPU.
    - Tt takes the new task’s context (saved earlier) and puts it into the CPU’s registers, program counter, etc.
    - This restores the new task to where it left off last time.

Example: Thread 2’s state (e.g., “at line 15, register B = pixel 50”) is loaded.

- Step 6: Start the New Task
    - The CPU begins running the new task.
    -  With the context loaded, the CPU jumps to the new task’s next instruction and starts executing.
    - The old task waits in memory; the new task takes over.

Example: Thread 2 starts updating the display for its 10ms time slice.

- Step 7: Repeat
    - This process loops—after Thread 2’s turn, the scheduler might switch back to Thread 1 or another task, saving and loading contexts each time.

Round Robin (RR), Time Sharing, and Shortest Job First (SJF) scheduling methods are ways a scheduler (the part of the operating system that decides which task runs on the CPU) manages multiple tasks.

Round Robin is a scheduling method where every task gets a turn to use the CPU for a fixed amount of time, one after another, in a circle. It’s like everyone gets a fair shot, no matter how long their work takes.

- Each task gets a time slice (e.g., 10 milliseconds) to run.
- The scheduler lines up all the tasks and lets them take turns in order.
- If a task isn’t done when its time is up, it goes to the back of the line and waits for its next turn.
- If a task finishes early, it leaves, and the next one steps up.

Example:
- Scenario: Three students (tasks) want to use the computer:
    - Student A: Needs 20ms to finish homework.
    - Student B: Needs 10ms to check email.
    - Student C: Needs 30ms to write a report.

Time Slice: 10ms per turn.

- Steps:
    - A runs for 10ms (10ms left), then stops.
    - B runs for 10ms (done, leaves).
    - C runs for 10ms (20ms left), then stops.
    - A runs for 10ms (done, leaves).
    - C runs for 10ms (10ms left), then stops.
    - C runs for 10ms (done, leaves).

Total Time: 60ms, but everyone got equal turns.

Time Sharing is a general idea where the CPU is split between multiple tasks so they all seem to run at once. It’s not a specific method but a goal—Round Robin is one way to do it.

- The scheduler quickly switches between tasks, giving each a small bit of time (like a few milliseconds).
- By swapping fast, it tricks you into thinking all tasks are running together, even on one CPU.
- It’s about making the system feel responsive for everyone using it.

Example
- Scenario: Same three students:
    - A (20ms), B (10ms), C (30ms).
- Time Sharing with RR (10ms slices, like above):
    - The teacher switches the computer between them every 10ms.
    - A gets 10ms, then B, then C, and repeats.
    - It feels like they’re all working at once, even thoughonly one uses the computer at a time.

Result: After 60ms, all finish, and no one waits too long to start.

Shortest Job First picks the task that will take the least time to finish and runs it first. It’s like letting the student with the quickest task cut the line.

- The scheduler looks at all waiting tasks and guesses how long each will take (needs to know this ahead of time).
- It runs the shortest one first, then the next shortest, and so on.
- If a new short task arrives, it might jump ahead of longer ones (in some versions, called “preemptive SJF”).

Scenario: Same three students:
- A: 20ms.
- B: 10ms (shortest).
- C: 30ms.

- Steps:
    - B runs for 10ms (done, shortest first).
    - A runs for 20ms (done, next shortest).
    - C runs for 30ms (done, longest last).

- Total Time: 60ms, but average wait time is lower:
    - B waits 0ms, A waits 10ms, C waits 30ms → Average = (0 + 10 + 30) / 3 = 13.3ms.

## 2 - Conpare between Process & Thread

### Context switching time
- Processes take longer because they use more resources
- Threads need less time because they are lighter than processes

### Shared memory
- When creating a process with fork(), the process and child process reside in 2 different allocated memory areas. Sharing data between them becomes more difficult
- Sharing data between threads in a process is faster and easier ecause they reside in the memory address saoace of process.
- Sharing data between processes is more diffcult. Through the IPC mechanism.
- Threas in a process can share data with each other quickly and easier.

Shared memory is a way for different parts of a program (threads or processes) to use the same chunk of memory to share information. It’s like a shared notepad where multiple people can read or write stuff to work together.

Threads are like teammates working inside the same “room” (a process). Because they’re part of the same process, they automatically share all the memory of that process—like having one big whiteboard everyone can see and use.

When a process starts, it gets a block of memory (its “address space”) for things like variables, data, and code.

All threads in that process use this same memory. If one thread writes something (like a number or a message), the others can read or change it instantly—no extra setup needed.

Processes are like separate workers in different “rooms.” Each process has its own private memory (its own whiteboard), and they don’t share it by default—the operating system keeps them isolated for safety.

To share memory, processes have to set it up using special tools provided by the system (like `shmget` or `mmap` in Linux).

This creates a specific chunk of memory that both processes can access—like putting a shared whiteboard in the hallway between their rooms.

They connect to this shared area, and then they can read or write to it, but their private memory stays separate.

### Crashed

If one process fails, other processes still execute normally.

If one thread fails, other threads in the same process terminate immediately.

## 3 - Operations on Thread
### Thread ID

A thread in a process is identified by a Thread ID.

- Process IDs are unique throughout the system, where Thread IDs are unique within a process
- A Process ID is an integer value but a Thread ID is a structure
- Process ID can be obtained very easily while Thread ID cannot

In C, using the pthread library, thread IDs are of type `pthread_t`. Here’s how to compare them:

Step 1: Get the Thread IDs
    - You can get a thread’s ID in two ways:
        - When Creating a Thread: The `pthread_create()` function gives you the TID of the new thread.
        - Inside a Thread: The `pthread_self()` function tells a thread its own TID.

Step 2: Compare Them
    - Use the `pthread_equal()` function to compare two `pthread_t` values. This is the safe and portable way because `pthread_t` might not just be a simple number—it could be a structure or something else depending on the system.

Why Not ==?
    - You shouldn’t directly use == (e.g., tid1 == tid2) because:
        - `pthread_t` isn’t guaranteed to be a plain integer—it might be complex (like a struct).
        - `pthread_equal()` handles the comparison correctly no matter how `pthread_t` is implemented.

```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h> // For sleep()

// Function that each thread runs
void* thread_function(void* arg) {
    pthread_t my_tid = pthread_self(); // Get my own thread ID
    pthread_t main_tid = *(pthread_t*)arg; // Get the main thread's ID (passed as argument)

    // Compare my TID with the main thread's TID
    if (pthread_equal(my_tid, main_tid)) {
        printf("I’m the main thread!\n");
    } else {
        printf("I’m a different thread, TID: %lu\n", (unsigned long)my_tid);
    }
    return NULL;
}

int main() {
    pthread_t main_tid = pthread_self(); // Main thread’s ID
    pthread_t thread1_tid, thread2_tid;  // IDs for new threads

    // Create two threads, passing the main thread's ID
    pthread_create(&thread1_tid, NULL, thread_function, &main_tid);
    pthread_create(&thread2_tid, NULL, thread_function, &main_tid);

    // Compare thread1_tid and thread2_tid
    if (pthread_equal(thread1_tid, thread2_tid)) {
        printf("Thread 1 and Thread 2 are the same (impossible here)!\n");
    } else {
        printf("Thread 1 and Thread 2 are different.\n");
    }

    // Run the main thread’s function too
    thread_function(&main_tid);

    // Wait for threads to finish
    pthread_join(thread1_tid, NULL);
    pthread_join(thread2_tid, NULL);

    return 0;
}
```

Output
```
Thread 1 and Thread 2 are different.
I’m the main thread!
I’m a different thread, TID: 123456789
I’m a different thread, TID: 987654321
```

### Create Thread
To create a new thread, we use `pthread_create()` function.

Every process has at least one thread. The thread containing the main function is called the main thread.

Return 0 if succeed, else, return a number bigger than 0

```c
int pthread_create(pthread_t threadID, const pthread_atr_t *attr, void *(*start)(void *), void *arg)
```

### Terminate Thread

- Thread ends normally (when thread finishes its execution)
- Thread terminates when calling `pthread_exit()` function from that thread
- Thread is canceled when calling `pthread_cancel()` function from another thread
- Any thread that calls the exit() function (when it returns), or the main thread terminates, all remaining threads terminate immediately.

### Example

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

pthread_t thread_id1, thread_id2;

typedef struct {
    char name[30];
    char msg[30];
} thr_data_t;

static void *thr_handle(void *args) 
{
    pthread_t tid = pthread_self();
    thr_data_t *data = (thr_data_t *)args;

    if (pthread_equal(tid, thread_id1)) {
        printf("I'm thread_id1\n\n");
    } else {
        printf("I'm thread_id2\n");
        printf("Hello %s, welcome to join %s\n", data->name, data->msg);
    }
}

static void *thr_handle2(void *args)
{
	printf("Hi im thread handle2\n");
}

int main(int argc, char const *argv[])
{
    /* code */
    int ret;
    thr_data_t data = {0};

    strncpy(data.name, "phonglt9", sizeof(data.name));                 
    strncpy(data.msg, "KMA thread programming\n", sizeof(data.msg));

    if (ret = pthread_create(&thread_id1, NULL, &thr_handle, NULL)) {
        printf("pthread_create() error number=%d\n", ret);
        return -1;
    }

    sleep(2);

    if (ret = pthread_create(&thread_id2, NULL, &thr_handle, &data)) {
        printf("pthread_create() error number=%d\n", ret);
        return -1;
    }

    sleep(5);

    return 0;
}
```

## 4 - Thread Management
### Thread Joining

Thread joining is when one thread waits for another thread to finish its work before moving on. It’s like telling a teammate, “I’ll wait here until you’re done, then we’ll continue together.”

In C, you use the `pthread_join()` function to make this happen.

Why use thread joining?
- Synchronization: You might need to wait for a thread to complete its task before the program does something else—like waiting for a calculation to finish before showing the result.
- Clean Exit: If the main thread (the one that starts in main()) exits before other threads finish, the whole program stops, and those threads might get cut off. Joining ensures everything wraps up properly.
- Get Results: You can grab a value a thread returns when it finishes (like a final answer it computed).

![alt text](_assets/threadJoining.png)

When you call `pthread_join()`:
- The calling thread (e.g., the main thread) pauses and waits.
- It waits until the target thread (the one you’re joining) finishes its job and exits.
- Once the target thread is done, the calling thread wakes up and keeps going.

```c
#include <stdio.h>
#include <pthread.h>

// Function the thread runs
void* thread_function(void* arg) {
    printf("Thread starting... I’ll work for 2 seconds.\n");
    sleep(2); // Pretend to work for 2 seconds
    printf("Thread finished!\n");
    return NULL; // Could return a value here
}

int main() {
    pthread_t my_thread; // Variable to hold the thread’s ID

    // Create a thread
    if (pthread_create(&my_thread, NULL, thread_function, NULL) != 0) {
        printf("Error creating thread!\n");
        return 1;
    }

    printf("Main thread: Waiting for my thread to finish...\n");

    // Wait for the thread to finish
    if (pthread_join(my_thread, NULL) != 0) {
        printf("Error joining thread!\n");
        return 1;
    }

    printf("Main thread: The thread is done, I can move on now!\n");

    return 0;
}
```

Output
```
Main thread: Waiting for my thread to finish...
Thread starting... I’ll work for 2 seconds.
[2-second pause]
Thread finished!
Main thread: The thread is done, I can move on now!
```
