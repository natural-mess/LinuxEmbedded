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
#include <sys/wait.h>
#include <pthread.h>
#include <ctype.h>
#include <time.h>
#include "log.h"
#include "sbuffer.h"
#include "../include/common.h"
#include "threads.h"
#include "keep_alive.h"

volatile sig_atomic_t shutdown_flag = 0;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "No port provided\nUsage: %s <port number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *endptr;
    errno = 0;
    long portNum = strtol(argv[1], &endptr, 10);

    if (errno == ERANGE || *endptr != '\0' || portNum < 1 || portNum > 65535)
    {
        fprintf(stderr, "Invalid port number: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    time_t now = time(NULL);
    char time_str[26];
    ctime_r(&now, time_str);
    time_str[strlen(time_str) - 1] = '\0';
    printf("%s: Sensor gateway started on port %ld\n", time_str, portNum);

    pid_t log_pid = fork();
    if (log_pid >= 0)
    {
        if (0 == log_pid)
        {
            log_process_run(LOG_FIFO, LOG_FIFO_PATH);
            exit(EXIT_SUCCESS);
        }
        else
        {
            char msg[256];
            snprintf(msg, sizeof(msg), "Sensor gateway started on port %d", (int)portNum);
            log_event(msg);

            sbuffer_t *sb = malloc(sizeof(sbuffer_t));
            if (sb == NULL)
            {
                log_event("Failed to allocate memory for sensor buffer in main");
                exit(EXIT_FAILURE);
            }

            if (sbuffer_init(sb, MAX_SENSORS) == -1)
            {
                log_event("Failed to initialize sensor buffer in main");
                free(sb);
                exit(EXIT_FAILURE);
            }

            init_threads(sb, (int)portNum);

            if (init_keep_alive() != 0)
            {
                log_event("Failed to init_keep_alive in main");
                sbuffer_free(sb);
                free(sb);
                exit(EXIT_FAILURE);
            }

            if (run_keep_alive() != 0)
            {
                log_event("Failed to run_keep_alive in main");
                sbuffer_free(sb);
                free(sb);
                exit(EXIT_FAILURE);
            }

            log_event("Shutdown");

            pthread_mutex_lock(&conn_mutex);
            shutdown_flag = 1;
            pthread_mutex_unlock(&conn_mutex);

            pthread_mutex_lock(&sb->mutex);
            pthread_cond_broadcast(&sb->not_empty);
            pthread_cond_broadcast(&sb->not_full);
            pthread_mutex_unlock(&sb->mutex);

            // Wait longer for threads to exit
            int max_wait = 10; // Increased to 10 seconds
            for (int i = 0; i < max_wait * 10; i++)
            {
                usleep(100000);
                int buffer_count = 0;
                if (sbuffer_count(sb, &buffer_count) == 0 && buffer_count == 0)
                {
                    break;
                }
            }

            if (pthread_mutex_destroy(&conn_mutex) != 0)
            {
                log_event("Failed to destroy conn_mutex in main");
            }

            if (sbuffer_free(sb) != 0)
            {
                log_event("Failed to free sbuffer in main");
            }

            free(sb);

            if (log_pid > 0)
            {
                kill(log_pid, SIGTERM);
                int status;
                if (waitpid(log_pid, &status, 0) == -1)
                {
                    perror("Failed to wait for log process");
                    log_event("Failed to wait for log process");
                }
                else
                {
                    log_event("Log process terminated");
                }
            }

            now = time(NULL);
            ctime_r(&now, time_str);
            time_str[strlen(time_str) - 1] = '\0';
            printf("%s: Sensor gateway shut down successfully\n", time_str);

            log_event("Sensor gateway shut down successfully");
            exit(EXIT_SUCCESS);
        }
    }
    else
    {
        perror("fork() failed");
        log_event("Failed to fork log process");
        exit(EXIT_FAILURE);
    }
}