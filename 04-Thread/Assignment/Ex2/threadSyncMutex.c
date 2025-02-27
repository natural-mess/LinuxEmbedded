#include <stdio.h>
#include <pthread.h>

// Shared resource
int counter = 0;

// Mutex to protect the counter
pthread_mutex_t mutex;

void* increment_counter(void* arg)
{
    for (int i = 0; i < 1000000; i++)
    {
        pthread_mutex_lock(&mutex);   // Lock the mutex
        counter++;                    // Critical section: increment counter
        pthread_mutex_unlock(&mutex); // Unlock the mutex
    }
    return NULL;
}

int main(int argc, char const *argv[])
{
    pthread_t thread1, thread2, thread3;

    // Initialize the mutex
    pthread_mutex_init(&mutex, NULL);

    // Create 3 threads
    pthread_create(&thread1, NULL, increment_counter, NULL);
    pthread_create(&thread2, NULL, increment_counter, NULL);
    pthread_create(&thread3, NULL, increment_counter, NULL);

    // Wait for threads to finish
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);

    // Clean up the mutex
    pthread_mutex_destroy(&mutex);

    printf("Final counter value: %d\n", counter);

    return 0;
}
