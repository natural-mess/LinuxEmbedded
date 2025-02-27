# Thread Exercises

## Exercise 1. Create and Run Basic Threads
**Description:** Write a program using the `pthread.h` library to perform the following operations:  
- Create two threads. Each thread will print a message string along with its thread ID (e.g., "Thread 1: Hello from thread").  
- Use `pthread_join` to wait for both threads to complete before the program terminates.  
**Questions:** How are `pthread_create` and `pthread_join` used? When does a thread terminate?  
**Hint:** Use `pthread_create` to create threads and `pthread_join` to wait for the threads to finish.

---

## Exercise 2. Thread Synchronization with Mutex
**Description:** Write a program using `pthread` and a mutex to perform the following operations:  
- Create a global variable `counter` initialized to 0.  
- Create three threads, each incrementing the `counter` by 1, 1,000,000 times.  
- Use a mutex to ensure that incrementing the `counter` is thread-safe.  
- Print the final value of `counter` after all threads complete.  
**Question:** Why is a mutex necessary in this exercise? What happens if the mutex is removed?  
**Hint:** Use `pthread_mutex_lock` and `pthread_mutex_unlock` to lock and unlock the mutex when accessing the `counter`.

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
