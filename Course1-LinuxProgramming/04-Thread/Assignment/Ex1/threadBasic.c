#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

static void *thr_handler(void *args) 
{
    pthread_t tid = pthread_self();
    int order = *(int *)args;

    if (order == 1)
    {
        printf("Hello from thread 1, my thread ID = %lu\n\n", (unsigned long)tid);
    } 
    else
    {
        printf("Hello from thread 2, my thread ID = %lu\n\n", (unsigned long)tid);
    }
    return NULL;
}

int main(int argc, char const *argv[])
{
    pthread_t thread_id1, thread_id2;
    int ret;
    int thread = 1;

    if (ret = pthread_create(&thread_id1, NULL, &thr_handler, &thread))
    {
        printf("pthread_create() error number=%d\n", ret);
        return -1;
    }

    sleep(2);
    thread = 2;
    if (ret = pthread_create(&thread_id2, NULL, &thr_handler, &thread))
    {
        printf("pthread_create() error number=%d\n", ret);
        return -1;
    }

    sleep(2);

    // Wait for threads to finish
    pthread_join(thread_id1, NULL);
    pthread_join(thread_id2, NULL);

    return 0;
}
