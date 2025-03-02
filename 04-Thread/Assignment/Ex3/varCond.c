#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

// Shared resource
int data = 0;
int count = 1; // Condition flag

// Mutex and condition variable
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void* producer(void* arg) 
{
    srand(time(NULL));
    for (int i = 0; i < 10; i++)
    {
        pthread_mutex_lock(&mutex); // Lock to update shared data
        data = 1 + rand() % 10;     // Generate a random number from 1 to 10
        pthread_cond_signal(&cond); // Send signal to waiting thread
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
    pthread_mutex_lock(&mutex);
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void* consumer(void* arg)
{
    // Lock to check condition
    pthread_mutex_lock(&mutex);
    while (count < 11)
    {
        // Wait and unlock mutex
        pthread_cond_wait(&cond, &mutex);
        printf("Consumer: Got data (%d), counter = %d\n", data, count++);
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main(int argc, char const *argv[])
{
    pthread_t prod, cons;

    // Create threads
    pthread_create(&prod, NULL, producer, NULL);
    pthread_create(&cons, NULL, consumer, NULL);

    // Wait for threads to finish
    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    return 0;
}
