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
#include "threads.h"
#include "keep_alive.h"

int main(int argc, char *argv[])
{
    // Read port number from command line
    if (argc < 2)
    {
        printf("No port provided\ncommand: ./sensor_gateway <port number>\n");
        exit(EXIT_FAILURE);
    }

    // Check port validity
    int portNum = atoi(argv[1]);
    if (portNum < 1024 || portNum > 65535 || !isdigit(portNum))
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
            // Log startup
            char msg[256];
            snprintf(msg, sizeof(msg), "Sensor gateway started on port %d", portNum);
            log_event(msg);

            // Initialize sensor buffer
            sbuffer_t *sb = malloc(sizeof(sbuffer_t));
            if (sbuffer_init(sb, MAX_SENSORS) == -1)
            {
                log_event("Failed to initialize sensor buffer in main");
                exit(EXIT_FAILURE);
            }

            // Create and init threads connection, data, storage
            init_threads(sb, portNum);

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