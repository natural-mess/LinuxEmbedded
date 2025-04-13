/** @file threads.c
 *  @brief Threads management
 *
 *  Manage the creation and detachment of the three threads 
 *  (connection, data, storage managers).
 *
 *  @author Phuc
 *  @bug No known bugs.
 */

#include <stdio.h>
#include <pthread.h>
#include "threads.h"
#include "log.h"
#include "connection_manager.h"
#include "data_manager.h"
#include "storage_manager.h"

void init_threads(sbuffer_t* sb, int port)
{
    // Create 3 threads: Connection manager, Data manager, and Storage manager
    pthread_t conn_thread, data_thread, stor_thread;
    int ret;

    // Allocate memory for thread arguments
    thread_args_t *conn_args = malloc(sizeof(thread_args_t));
    thread_args_t *data_args = malloc(sizeof(thread_args_t));
    thread_args_t *stor_args = malloc(sizeof(thread_args_t));

    if (!conn_args || !data_args || !stor_args)
    {
        log_event("Failed to allocate memory for thread arguments");
        free(conn_args);
        free(data_args);
        free(stor_args);
        exit(EXIT_FAILURE);
    }

    // Initialize thread arguments
    conn_args->sb = sb;
    conn_args->port = port;

    data_args->sb = sb;
    data_args->port = port;

    stor_args->sb = sb;
    stor_args->port = port;

    // Connection manager thread
    ret = pthread_create(&conn_thread, NULL, &connection_manager, conn_args);
    if (ret != 0)
    {
        printf("pthread_create() Connection manager error number=%d\n", ret);
        log_event("pthread_create() Connection manager failed");
        free(conn_args);
        free(data_args);
        free(stor_args);
        exit(EXIT_FAILURE);
    }
    ret = pthread_detach(conn_thread);
    if (ret != 0)
    {
        printf("pthread_detach() Connection manager error number=%d\n", ret);
        log_event("pthread_detach() Connection manager failed");
        free(conn_args);
        free(data_args);
        free(stor_args);
        exit(EXIT_FAILURE);
    }

    // Data manager thread
    ret = pthread_create(&data_thread, NULL, &data_manager, data_args);
    if (ret != 0)
    {
        printf("pthread_create() Data manager error number=%d\n", ret);
        log_event("pthread_create() Data manager failed");
        free(conn_args);
        free(data_args);
        free(stor_args);
        exit(EXIT_FAILURE);
    }
    ret = pthread_detach(data_thread);
    if (ret != 0)
    {
        printf("pthread_detach() Data manager error number=%d\n", ret);
        log_event("pthread_detach() Data manager failed");
        free(conn_args);
        free(data_args);
        free(stor_args);
        exit(EXIT_FAILURE);
    }

    // Storage manager thread
    ret = pthread_create(&stor_thread, NULL, &storage_manager, stor_args);
    if (ret != 0)
    {
        printf("pthread_create() Storage manager error number=%d\n", ret);
        log_event("pthread_create() Storage manager failed");
        free(conn_args);
        free(data_args);
        free(stor_args);
        exit(EXIT_FAILURE);
    }
    ret = pthread_detach(stor_thread);
    if (ret != 0)
    {
        printf("pthread_detach() Storage manager error number=%d\n", ret);
        log_event("pthread_detach() Storage manager failed");
        free(conn_args);
        free(data_args);
        free(stor_args);
        exit(EXIT_FAILURE);
    }
}