/** @file common.h
 *  @brief Common definitions (e.g., data structs)
 *
 *  Contains node data format that are used
 *  throughout project
 *
 *  @author Phuc
 *  @bug No known bugs.
 */

#ifndef _COMMON_H
#define _COMMON_H

#include "../src/sbuffer.h"

#define MAX_SENSORS 50
#define TIMEOUT_SECONDS 15

typedef struct
{
    sbuffer_t* sb;
    int port;
} thread_args_t;

typedef struct
{
    int connection_id;
    char ip[16];
    int port;
    time_t last_active;
    int active;
} connection_tracking_t;

extern int conn_active_count;
extern pthread_mutex_t conn_mutex;

volatile sig_atomic_t keep_running = 1;

#endif /* _COMMON_H */
