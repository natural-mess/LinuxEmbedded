/** @file keep_alive.h
 *  @brief Keep-Alive and Signal Handling
 *
 *  Manage the keep-alive loop, connection tracking, and signal handling.
 *
 *  @author Phuc
 *  @bug No known bugs.
 */

#ifndef KEEP_ALIVE_H
#define KEEP_ALIVE_H

#include <pthread.h>
#include "common.h"

typedef struct
{
    int connection_id;
    char ip[16];
    int port;
    time_t last_active;
    int active;
} connection_tracking_t;

extern connection_tracking_t connections[MAX_SENSORS];
extern int conn_active_count;
extern pthread_mutex_t conn_mutex;

int init_keep_alive(void);
int run_keep_alive(void);
void remove_connection(int index);

#endif /* KEEP_ALIVE_H */