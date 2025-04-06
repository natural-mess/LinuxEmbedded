/** @file connection_manager.h
 *  @brief connection_manager definitions
 *
 *  Declare the connection manager thread function and any supporting types.
 *
 *  @author Phuc
 *  @bug No known bugs.
 */

#include <pthread.h>
#include "../include/common.h"
#include "log.h"
#include "sbuffer.h"
#include "keep_alive.h"

#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H

// Handle socket creation, binding, and listening.
int setup_socket(int port);

// Manage accept and add to client_fds/connections
void handle_new_connection(int socket_fd, int* client_fds, int* client_count, fd_set* readfds);

// Read data, push to sbuffer, update last_active, close if needed.
void handle_client_data(int* client_fds, int* client_count, sbuffer_t* sb, fd_set* readfds);

// Close all FDs on shutdown.
void cleanup_connections(int* client_fds, int client_count, int socket_fd);

// Shift array when one element is deleted
void shift_clients(int* client_fds, int* client_count, int index);

// Coordinate these functions in the main loop.
void* connection_manager(void* arg);

#endif /* CONNECTION_MANAGER_H */
