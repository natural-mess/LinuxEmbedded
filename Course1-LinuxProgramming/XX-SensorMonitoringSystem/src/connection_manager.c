/** @file connection_manager.c
 *  @brief Implementation of connection manager
 *
 *  Handles connection manager thread
 *
 *  @author Phuc
 *  @bug No known bugs.
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
#include <time.h>
#include <string.h>
#include "../include/common.h"
#include "keep_alive.h"
#include "connection_manager.h"
#include "threads.h"

// Handle socket creation, binding, and listening.
int setup_socket(int port)
{
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        perror("Failed to create TCP socket");
        log_event("Failed to create TCP socket");
        return -1;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        close(socket_fd);
        perror("Failed to bind TCP socket");
        log_event("Failed to bind TCP socket");
        return -1;
    }

    if (listen(socket_fd, SOMAXCONN) == -1)
    {
        close(socket_fd);
        perror("Failed to listen TCP socket");
        log_event("Failed to listen TCP socket");
        return -1;
    }

    return socket_fd;
}

// Manage accept and add to client_fds/connections
void handle_new_connection(int socket_fd, int *client_fds, int *client_count, fd_set *readfds)
{
    if (shutdown_flag)
        return;

    char msg[256];

    if (FD_ISSET(socket_fd, readfds))
    {
        int client_fd = accept(socket_fd, NULL, NULL);
        if (client_fd == -1)
        {
            log_event("Failed to accept TCP socket");
            return;
        }

        if (*client_count < MAX_SENSORS)
        {
            client_fds[(*client_count)++] = client_fd;

            if (pthread_mutex_lock(&conn_mutex) != 0)
            {
                perror("Conn mutex lock failed connection manager");
                log_event("Mutex lock failed in connection manager");
                return;
            }

            connections[conn_active_count].connection_id = client_fd;
            connections[conn_active_count].last_active = time(NULL);
            connections[conn_active_count].active = 1;
            conn_active_count++;

            if (pthread_mutex_unlock(&conn_mutex) != 0)
            {
                perror("Conn mutex unlock failed in connection manager");
                log_event("Mutex unlock failed in connection manager");
                return;
            }

            snprintf(msg, sizeof(msg), "A sensor node with %d has opened a new connection", client_fd);
            log_event(msg);
            // Print to terminal
            time_t now = time(NULL);
            char time_str[26];
            ctime_r(&now, time_str);
            time_str[strlen(time_str) - 1] = '\0';
            printf("%s: Connection %d established\n", time_str, client_fd);
        }
        else
        {
            log_event("Max client reached");
            close(client_fd);
        }
    }
}

// Shift array when one element is deleted
void shift_clients(int *client_fds, int *client_count, int index)
{
    client_fds[index] = -1;
    for (int j = index; j < *client_count - 1; j++)
    {
        client_fds[j] = client_fds[j + 1];
    }
    (*client_count)--;
}

// Read data, push to sbuffer, update last_active, close if needed.
void handle_client_data(int *client_fds, int *client_count, sbuffer_t *sb, fd_set *readfds)
{
    if (shutdown_flag)
        return;

    char msg[256];

    for (int i = 0; i < *client_count; i++)
    {
        if (FD_ISSET(client_fds[i], readfds))
        {
            sensor_data_t sdata;
            ssize_t bytes = read(client_fds[i], &sdata, sizeof(sensor_data_t));
            if (bytes > 0)
            {
                snprintf(msg, sizeof(msg), "Received data: sensor_id=%d, temp=%.2f, time=%ld",
                         sdata.sensor_id, sdata.temperature, sdata.timestamp);
                log_event(msg);

                if (sbuffer_push(sb, sdata) != 0)
                {
                    log_event("Failed to push data to sbuffer");
                }
                else
                {
                    log_event("Data successfully pushed to sbuffer");
                }

                if (pthread_mutex_lock(&conn_mutex) != 0)
                {
                    perror("Conn mutex lock failed connection manager");
                    log_event("Mutex lock failed in connection manager");
                    return;
                }

                connections[i].last_active = time(NULL);

                if (pthread_mutex_unlock(&conn_mutex) != 0)
                {
                    perror("Conn mutex unlock failed in connection manager");
                    log_event("Mutex unlock failed in connection manager");
                    return;
                }
            }
            else if (bytes == 0)
            {
                snprintf(msg, sizeof(msg), "The sensor node with %d has closed the connection", client_fds[i]);
                log_event(msg);
                // Print to terminal
                time_t now = time(NULL);
                char time_str[26];
                ctime_r(&now, time_str);
                time_str[strlen(time_str) - 1] = '\0';
                printf("%s: Connection %d closed\n", time_str, client_fds[i]);

                if (pthread_mutex_lock(&conn_mutex) != 0)
                {
                    perror("Conn mutex lock failed connection manager");
                    log_event("Mutex lock failed in connection manager");
                    return;
                }

                remove_connection(i);

                if (pthread_mutex_unlock(&conn_mutex) != 0)
                {
                    perror("Conn mutex unlock failed in connection manager");
                    log_event("Mutex unlock failed in connection manager");
                    return;
                }

                shift_clients(client_fds, client_count, i);
                i--;
            }
            else
            {
                snprintf(msg, sizeof(msg), "Failed to read from sensor node %d", client_fds[i]);
                log_event(msg);

                if (pthread_mutex_lock(&conn_mutex) != 0)
                {
                    perror("Conn mutex lock failed connection manager");
                    log_event("Mutex lock failed in connection manager");
                    return;
                }
                remove_connection(i);
                if (pthread_mutex_unlock(&conn_mutex) != 0)
                {
                    perror("Conn mutex unlock failed in connection manager");
                    log_event("Mutex unlock failed in connection manager");
                    return;
                }

                shift_clients(client_fds, client_count, i);
                i--;
            }
        }
    }
}

// Close all FDs on shutdown.
void cleanup_connections(int *client_fds, int client_count, int socket_fd)
{
    if (pthread_mutex_lock(&conn_mutex) != 0)
    {
        perror("Conn mutex lock failed connection manager");
        log_event("Mutex lock failed in connection manager");
        return;
    }

    for (int i = 0; i < client_count; i++)
    {
        if (client_fds[i] != -1)
        {
            close(client_fds[i]);
        }
    }
    conn_active_count = 0;

    if (pthread_mutex_unlock(&conn_mutex) != 0)
    {
        perror("Conn mutex unlock failed in connection manager");
        log_event("Mutex unlock failed in connection manager");
        return;
    }

    close(socket_fd);
    log_event("Connection manager shutting down");
}

// Coordinate these functions in the main loop.
void *connection_manager(void *arg)
{
    thread_args_t *data = (thread_args_t *)arg;

    char msg[256];
    snprintf(msg, sizeof(msg), "Connection manager started on port %d", data->port);
    log_event(msg);

    int socket_fd = setup_socket(data->port);
    if (socket_fd < 0)
    {
        perror("Failed to setup socket");
        log_event("Failed to setup socket");
        exit(EXIT_FAILURE);
    }

    int client_fds[MAX_SENSORS] = {-1};
    int client_count = 0;
    fd_set readfds;

    while (!shutdown_flag)
    {
        FD_ZERO(&readfds);
        FD_SET(socket_fd, &readfds);
        int max_fd = socket_fd;

        for (int i = 0; i < client_count; i++)
        {
            if (client_fds[i] != -1)
            {
                FD_SET(client_fds[i], &readfds);
                if (client_fds[i] > max_fd)
                {
                    max_fd = client_fds[i];
                }
            }
        }

        int select_result = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        if (select_result > 0)
        {
            handle_new_connection(socket_fd, client_fds, &client_count, &readfds);
            handle_client_data(client_fds, &client_count, data->sb, &readfds);
        }
        else if (select_result == -1 && !shutdown_flag)
        {
            log_event("Select failed");
        }
        else if (select_result == 0)
        {
            continue;
        }
    }

    cleanup_connections(client_fds, client_count, socket_fd);

    return NULL;
}