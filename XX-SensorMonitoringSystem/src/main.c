/** @file main.c
 *  @brief Implementation of the main process
 *
 *  Initializing the main process, forking the log process,
 *  and creating the three required threads
 *  (connection manager, data manager, and storage manager)
 *
 *  @author Phuc
 *  @bug No known bugs.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <pthread.h>
#include "log.h"
#include "sbuffer.h"
#include "../include/common.h"
#include "connection_manager.h"
#include "data_manager.h"
#include "storage_manager.h"

void sigint_handler(int sig)
{
    keep_running = 0;
    write(STDERR_FILENO, "Shutdown signal received\n", 25);
}

void keep_alive(void)
{
    connection_tracking_t connections[MAX_SENSORS];
    for (int i = 0; i < MAX_SENSORS; i++)
    {
        connections[i].active = 0;
    }

    if (signal(SIGINT, sigint_handler) == SIG_ERR)
    {
        perror("Cannot handle SIGINT");
        exit(EXIT_FAILURE);
    }

    while (keep_running)
    {
        sleep(10);
        pthread_mutex_lock(&conn_mutex);
        for (int i = 0; i < conn_active_count; i++)
        {
            if ((connections[i].active == 1) && (time(NULL) - connections[i].last_active > TIMEOUT_SECONDS))
            {
                char msg[256];
                snprintf(msg, sizeof(msg), "Sensor node with %d has disconnected (keep-alive timeout)", connections[i].connection_id);
                log_event(msg);
                printf("Sensor node with %d has disconnected (keep-alive timeout)", connections[i].connection_id);
                connections[i].active = 0;
                for (int j = i; j < conn_active_count - 1; j++)
                {
                    connections[j] = connections[j + 1];
                }
                conn_active_count--;
                i--;
            }
        }
        pthread_mutex_unlock(&conn_mutex);
    }
}

int main(int argc, char *argv[])
{
    // Read port number from command line
    if (argc < 2)
    {
        printf("No port provided\ncommand: ./sensor_gateway <port number>\n");
        exit(EXIT_FAILURE);
    }

    int portNum = atoi(argv[1]);
    if (portNum < 1024 || portNum > 65535)
    {
        perror("Invalid port number");
        exit(EXIT_FAILURE);
    }

    // Create processes
    pid_t log_pid = fork();
    if (log_pid >= 0)
    {
        // Log process (child process)
        if (0 == log_pid)
        {
            log_process_run(LOG_FIFO, LOG_FIFO_PATH);
        }
        else
        {
            // Initialize sensor buffer
            sbuffer_t *sb = malloc(sizeof(sbuffer_t));
            if (sbuffer_init(sb, MAX_SENSORS) == -1)
            {
                log_event("Failed to initialize sensor buffer in main");
                exit(EXIT_FAILURE);
            }

            // Init connection mutex
            if (pthread_mutex_init(&conn_mutex, NULL) != 0)
            {
                perror("Init mutex in main failed");
                exit(EXIT_FAILURE);
            }

            // Create 3 threads: Connection manager, Data manager and Storage manager
            pthread_t conn_thread, data_thread, stor_thread;
            thread_args_t thread_args;
            thread_args.sb = sb;
            thread_args.port = portNum;
            int ret;

            // Connection manager thread
            ret = pthread_create(&conn_thread, NULL, &connection_manager, &thread_args);
            if (ret != 0)
            {
                printf("pthread_create() Connection manager error number=%d\n", ret);
                log_event("pthread_create() Connection manager failed");
                exit(EXIT_FAILURE);
            }
            ret = pthread_detach(conn_thread);
            if (ret != 0)
            {
                printf("pthread_detach() Connection manager error number=%d\n", ret);
                log_event("pthread_detach() Connection manager failed");
                exit(EXIT_FAILURE);
            }

            // Data manager thread
            ret = pthread_create(&data_thread, NULL, &data_manager, &thread_args);
            if (ret != 0)
            {
                printf("pthread_create() Data manager error number=%d\n", ret);
                log_event("pthread_create() Data manager failed");
                exit(EXIT_FAILURE);
            }
            ret = pthread_detach(data_thread);
            if (ret != 0)
            {
                printf("pthread_detach() Data manager error number=%d\n", ret);
                log_event("pthread_detach() Data manager failed");
                exit(EXIT_FAILURE);
            }

            // Storage manager thread
            ret = pthread_create(&stor_thread, NULL, &storage_manager, &thread_args);
            if (ret != 0)
            {
                printf("pthread_create() Storage manager error number=%d\n", ret);
                log_event("pthread_create() Storage manager failed");
                exit(EXIT_FAILURE);
            }
            ret = pthread_detach(stor_thread);
            if (ret != 0)
            {
                printf("pthread_detach() Storage manager error number=%d\n", ret);
                log_event("pthread_detach() Storage manager failed");
                exit(EXIT_FAILURE);
            }

            // Connection tracking for keep-alive
            keep_alive();

            // Clean up
            log_event("Shutdown signal received");
            pthread_mutex_destroy(&conn_mutex)
            log_event("Shutdown");
            sbuffer_free(sb);
            free(sb);
            exit(EXIT_SUCCESS);
        }
    }
    else
    {
        perror("fork() failed"); // fork() return -1 if fail
        log_event("Failed to fork log process");
        exit(EXIT_FAILURE);
    }
}
