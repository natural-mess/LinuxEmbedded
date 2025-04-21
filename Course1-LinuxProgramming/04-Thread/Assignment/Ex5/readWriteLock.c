#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// Shared data
int data = 0;

// Read-write lock
pthread_rwlock_t rwlock;

void* reader(void* arg)
{
    int id = *(int*)arg;
    pthread_rwlock_rdlock(&rwlock); // Lock for reading
    printf("Reader %d: Data is %d\n", id, data);
    sleep(1);
    pthread_rwlock_unlock(&rwlock); // Unlock
    return NULL;
}

void* writer(void* arg)
{
    int id = *(int*)arg;
    pthread_rwlock_wrlock(&rwlock); // Lock for writing
    data++;
    printf("Writer %d: Incremented data to %d\n", id, data);
    sleep(1);
    pthread_rwlock_unlock(&rwlock); // Unlock
    return NULL;
}

int main(int argc, char const *argv[])
{
    pthread_t readers[3], writers[2];
    int reader_ids[3] = {1, 2, 3};
    int writer_ids[2] = {4, 5};

    // Initialize read-write lock
    pthread_rwlock_init(&rwlock, NULL);

    // Create 3 reader threads
    for (int i = 0; i < 3; i++)
    {
        pthread_create(&readers[i], NULL, reader, &reader_ids[i]);
    }

    // Create 2 writer threads
    pthread_create(&writers[0], NULL, writer, &writer_ids[0]);
    pthread_create(&writers[1], NULL, writer, &writer_ids[1]);

    // Wait for threads to finish
    for (int i = 0; i < 3; i++)
    {
        pthread_join(readers[i], NULL);
    }
    pthread_join(writers[0], NULL);
    pthread_join(writers[1], NULL);

    // Clean up lock
    pthread_rwlock_destroy(&rwlock);

    printf("Final data value: %d\n", data);
    return 0;
}