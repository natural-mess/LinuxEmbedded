/** @file keep_alive.c
 *  @brief Keep-Alive and Signal Handling
 *
 *  Manage the keep-alive loop, connection tracking, and signal handling.
 *
 *  @author Phuc
 *  @bug No known bugs.
 */

#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include "../include/common.h"
#include "keep_alive.h"
#include "log.h"

connection_tracking_t connections[MAX_SENSORS];
int conn_active_count = 0;
pthread_mutex_t conn_mutex;

static void sigint_handler(int sig)
{
    pthread_mutex_lock(&conn_mutex);
    shutdown_flag = 1;
    pthread_mutex_unlock(&conn_mutex);
    
    log_event("Received SIGINT, initiating shutdown");
    write(STDERR_FILENO, "Shutdown signal received\n", 25);
}

void remove_connection(int index)
{
    if (index < 0 || index >= conn_active_count)
    {
        perror("Invalid index in remove_connection");
        return;
    }

    if (conn_active_count <= 0)
    {
        perror("conn_active_count is 0 in remove_connection");
        return;
    }

    for (int i = index; i < conn_active_count - 1; i++)
    {
        connections[i] = connections[i + 1];
    }
    conn_active_count--;
}

int init_keep_alive(void)
{
    // Init connection mutex
    if (pthread_mutex_init(&conn_mutex, NULL) != 0)
    {
        perror("Init mutex in init_keep_alive failed");
        return -1;
    }

    // Register exit signal
    if (signal(SIGINT, sigint_handler) == SIG_ERR)
    {
        perror("Cannot handle SIGINT");
        return -1;
    }

    // Clear connection tracking for keep-alive
    for (int i = 0; i < MAX_SENSORS; i++)
    {
        connections[i].active = 0;
    }

    return 0;
}

int run_keep_alive(void)
{
    while (!shutdown_flag)
    {
        sleep(10);

        if (pthread_mutex_lock(&conn_mutex) != 0)
        {
            perror("Conn mutex lock failed in keep_alive");
            log_event("Mutex lock failed in keep_alive");
            return -1;
        }

        for (int i = 0; i < conn_active_count; i++)
        {
            if ((connections[i].active == 1) && (time(NULL) - connections[i].last_active > TIMEOUT_SECONDS))
            {
                char msg[256];
                snprintf(msg, sizeof(msg), "Sensor node with %d has disconnected (keep-alive timeout)", connections[i].connection_id);
                log_event(msg);

                // Print to terminal
                time_t now = time(NULL);
                char time_str[26];
                ctime_r(&now, time_str);
                time_str[strlen(time_str) - 1] = '\0';
                printf("%s: Connection %d closed (timeout)\n", time_str, connections[i].connection_id);
                connections[i].active = 0;
                remove_connection(i);
                i--;
            }
        }

        if (pthread_mutex_unlock(&conn_mutex) != 0)
        {
            perror("Conn mutex unlock failed in keep_alive");
            log_event("Mutex unlock failed in keep_alive");
            return -1;
        }
    }

    return 0;
}
