#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>

#define ARR_SIZE            1000000
#define ARR_SIZE_DEVIDED    (ARR_SIZE/4)
#define RANDOM_SEED         1000

int arr[ARR_SIZE];
long long sum = 0;

void array_init(void)
{
    srand(time(NULL));
    for (int i = 0; i < ARR_SIZE; i++)
    {
        arr[i] = 1 + rand() % RANDOM_SEED;
    }
}

void print_array(void)
{
    for (int i = 0; i < ARR_SIZE; i++)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

long long checkSumResult(void)
{
    long long res = 0;
    for (int i = 0; i < ARR_SIZE; i++)
    {
        res = res + arr[i];
    }
    return res;
}

// Mutex to protect the counter
pthread_mutex_t mutex;

void* calc_sum(void* arg)
{
    int begin_index = *(int*)arg;
    long long localSum = 0;
    for (int i = begin_index; i < (begin_index + ARR_SIZE_DEVIDED); i++)
    {
        localSum = localSum + (long long)arr[i];
    }
    pthread_mutex_lock(&mutex);   // Lock the mutex
    sum += localSum;
    pthread_mutex_unlock(&mutex); // Unlock the mutex
    return NULL;
}

int main(int argc, char const *argv[])
{
    pthread_t thread1, thread2, thread3, thread4;
    int firstIndex[4] = {0};

    for (int i = 1; i < 4; i++)
    {
        firstIndex[i] = firstIndex[i-1] + ARR_SIZE_DEVIDED;
    }

    array_init();
    print_array();

    // Initialize the mutex
    pthread_mutex_init(&mutex, NULL);

    // Create 4 threads
    pthread_create(&thread1, NULL, calc_sum, &firstIndex[0]);
    pthread_create(&thread2, NULL, calc_sum, &firstIndex[1]);
    pthread_create(&thread3, NULL, calc_sum, &firstIndex[2]);
    pthread_create(&thread4, NULL, calc_sum, &firstIndex[3]);

    // Wait for threads to finish
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);
    pthread_join(thread4, NULL);

    // Clean up the mutex
    pthread_mutex_destroy(&mutex);

    printf("Sum result from threads value: %lld\n", sum);
    printf("Sum result from raw calculation: %lld\n", checkSumResult());

    return 0;
}
