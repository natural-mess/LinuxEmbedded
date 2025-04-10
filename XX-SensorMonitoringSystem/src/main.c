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
#include "log.h"
#include "sbuffer.h"
#include "../include/common.h"
#include "threads.h"
#include "keep_alive.h"

volatile sig_atomic_t shutdown_flag = 0;

int main(int argc, char *argv[])
{
    // Read port number from command line
    if (argc < 2)
    {
        fprintf(stderr, "No port provided\nUsage: %s <port number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *endptr;
    errno = 0;
    long portNum = strtol(argv[1], &endptr, 10); // Use strtol for robust conversion

    // Check for conversion errors
    if (errno == ERANGE || *endptr != '\0' || portNum < 1 || portNum > 65535)
    {
        fprintf(stderr, "Invalid port number: %s\n", argv[1]);
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
            exit(EXIT_SUCCESS);
        }
        else
        {
            // Log startup
            char msg[256];
            snprintf(msg, sizeof(msg), "Sensor gateway started on port %d", (int)portNum);
            log_event(msg);

            // Initialize sensor buffer
            sbuffer_t *sb = malloc(sizeof(sbuffer_t));
            if (sbuffer_init(sb, MAX_SENSORS) == -1)
            {
                log_event("Failed to initialize sensor buffer in main");
                exit(EXIT_FAILURE);
            }

            // Create and init threads connection, data, storage
            init_threads(sb, (int)portNum);

            // Init keep-alive
            if (init_keep_alive() != 0)
            {
                log_event("Failed to init_keep_alive in main");
                exit(EXIT_FAILURE);
            }

            // Run keep-alive while loop
            if (run_keep_alive() != 0)
            {
                log_event("Failed to run_keep_alive in main");
                exit(EXIT_FAILURE);
            }

            // Clean up
            // Log shutdown event
            log_event("Shutdown");

            // Destroy connection mutex
            if (pthread_mutex_destroy(&conn_mutex) != 0)
            {
                log_event("Failed to destroy conn_mutex in main");
                exit(EXIT_FAILURE);
            }

            // Free sensor buffer
            if (sbuffer_free(sb) != 0)
            {
                log_event("Failed to free sbuffer in main");
                exit(EXIT_FAILURE);
            }

            free(sb);

            // Wait for and clean up log process
            if (log_pid > 0)
            {
                kill(log_pid, SIGTERM); // Ensure child is signaled to exit
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

            log_event("Sensor gateway shut down successfully");
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
