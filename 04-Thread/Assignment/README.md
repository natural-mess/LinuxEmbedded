# Thread Exercises

## Exercise 1. Create and Run Basic Threads
**Description:** Write a program using the `pthread.h` library to perform the following operations:  
- Create two threads. Each thread will print a message string along with its thread ID (e.g., "Thread 1: Hello from thread").  
- Use `pthread_join` to wait for both threads to complete before the program terminates.  
**Questions:** How are `pthread_create` and `pthread_join` used? When does a thread terminate?  
**Hint:** Use `pthread_create` to create threads and `pthread_join` to wait for the threads to finish.

`pthread_create` is called twice to make two separate threads:
- Thread 1 runs `thr_handler` with order = 1.
- Thread 2 runs `thr_handler` with order = 2.

The thread variable acts as a flag to tell each thread which one it is (1 or 2). Main passes `&thread` (its address) so each thread can read the value at that moment:
- Thread 1 sees 1 (before thread changes).
- Thread 2 sees 2 (after thread is updated).

A thread terminates when it:
- Returns from its function: In `thr_handler`, return `NULL`; ends the thread.
- Calls `pthread_exit()`: Explicitly stops it (not used here, but similar effect).
- Once it ends, its resources (like its ID) linger until joined or detached.

Both threads terminate before pthread_join in this case (due to sleep(2) delays in main), but pthread_join still:
- Waits if they’re not done yet (not needed here—they’re fast).
- Cleans up their resources (no zombies).

To run the code:

```bash
cd Ex1/
```
```bash
make all
```
```bash
./threadBasic
```

---

## Exercise 2. Thread Synchronization with Mutex
**Description:** Write a program using `pthread` and a mutex to perform the following operations:  
- Create a global variable `counter` initialized to 0.  
- Create three threads, each incrementing the `counter` by 1, 1,000,000 times.  
- Use a mutex to ensure that incrementing the `counter` is thread-safe.  
- Print the final value of `counter` after all threads complete.  
**Question:** Why is a mutex necessary in this exercise? What happens if the mutex is removed?  
**Hint:** Use `pthread_mutex_lock` and `pthread_mutex_unlock` to lock and unlock the mutex when accessing the `counter`.

`counter++`: This looks like one operation, but it’s non-atomic—it’s actually three steps:
- Read counter’s current value.
- Add 1 to it.
- Write the new value back.

Concurrency: These threads run at the same time (multi-core) or switch rapidly (single-core). Without control, they can overlap these steps, leading to a race condition.

Example :
- Starting counter = 0.
- Thread 1: Reads 0, prepares to write 1.
- Thread 2: Reads 0 (before Thread 1 writes), prepares to write 1.
- Thread 3: Reads 0 (still 0), prepares to write 1.
- All write 1—counter becomes 1, but three increments happened!
- Lost Updates: Only one increment sticks; the others are overwritten.

To run the code:

```bash
cd Ex2/
```
```bash
make all
```
```bash
./threadSyncMutex
```

---

## Exercise 3. Using Condition Variables
**Description:** Write a program with two threads: a producer and a consumer, as follows:  
- **Producer:** Generates a random number from 1 to 10 and stores it in a global variable `data`.  
- **Consumer:** Waits for the producer to provide data, then reads and prints it.  
- Use `pthread_cond_wait` and `pthread_cond_signal` to synchronize the producer and consumer, ensuring the consumer only reads data after the producer has provided it.  
- Repeat the process 10 times and print all values read by the consumer.  
**Hint:** Use `pthread_cond_wait` to make the consumer wait until the producer signals that the data is ready.



---

## Exercise 4. Count Even and Odd Numbers with Threads
**Description:** Write a program to perform the following steps:  
- Create a random array containing 100 integers from 1 to 100.  
- Create two threads: one to count even numbers and one to count odd numbers in the array.  
- After both threads finish counting, print the total number of even and odd numbers.  
- Use `pthread_join` to ensure the program only terminates after both threads complete their tasks.  
**Hint:** Each thread handles a distinct task, so a mutex is not needed in this exercise.

---

## Exercise 5. Resource Sharing with Read-Write Lock
**Description:** Write a program simulating a scenario with multiple threads reading and writing data:  
- Create a variable `data` containing an integer.  
- Create 5 reader threads and 2 writer threads:  
  - Reader threads only read `data` without modifying it.  
  - Writer threads increment the value of `data` by 1.  
- Use a read-write lock to ensure multiple threads can read simultaneously, but only one thread can write at any given time.  
- Print the value of `data` after all threads complete.  
**Hint:** Use `pthread_rwlock_rdlock` and `pthread_rwlock_wrlock` for synchronized read-write operations.

---

## Exercise 6. Calculate Array Sum with Threads and Mutex
**Description:** Write a program to calculate the sum of a large array containing 1 million integers.  
- Divide the array into 4 equal parts.  
- Create 4 threads, each calculating the sum of one part of the array.  
- Use a global sum variable and a mutex to aggregate results from all threads.  
- Print the total sum of the array after the threads complete.  
**Hint:** Use `pthread_mutex_lock` to protect the global sum variable when threads add their results.
