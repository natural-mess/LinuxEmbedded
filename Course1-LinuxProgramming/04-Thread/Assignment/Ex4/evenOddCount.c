#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>

#define ARR_SIZE 100

void *thr_handler_odd(void *args)
{
    int countOdd = 0;
    int *data = (int *)args;
    for (int i = 0; i < ARR_SIZE; i++)
    {
        if ((data[i] % 2) != 0)
        {
            countOdd++;
        }
    }
    printf("Odd number count = %d\n", countOdd);
    return NULL;
}

void *thr_handler_even(void *args)
{
    int countEven = 0;
    int *data = (int *)args;
    for (int i = 0; i < ARR_SIZE; i++)
    {
        if ((data[i] % 2) == 0)
        {
            countEven++;
        }
    }
    printf("Even number count = %d\n", countEven);
    return NULL;
}

int main(int argc, char const *argv[])
{
    pthread_t thCountOdd, thCountEven;
    int arr[ARR_SIZE];
    srand(time(NULL));
    for (int i = 0; i < ARR_SIZE; i++)
    {
        arr[i] = 1 + rand() % 100; // generate random number from 1 to 100
        printf("%d ", arr[i]);
    }
    printf("\n");

    // Create threads
    pthread_create(&thCountOdd, NULL, thr_handler_odd, arr);
    pthread_create(&thCountEven, NULL, thr_handler_even, arr);

    // Wait for threads to finish
    pthread_join(thCountOdd, NULL);
    pthread_join(thCountEven, NULL);

    return 0;
}
