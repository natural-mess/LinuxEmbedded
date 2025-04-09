/** @file data_manager.h
 *  @brief Data manager declarations
 *
 *  Declare the data manager thread function and any supporting types.
 *
 *  @author Phuc
 *  @bug No known bugs.
 */

#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include "../include/common.h"
#include "sbuffer.h"
#include "log.h"
#include "threads.h"

#define TOO_HOT 40.0
#define TOO_COLD 18.0

typedef struct
{
    float sum;        // Sum of temperatures for running average
    int count;        // Number of readings
    time_t last_update; // Last time updated (optional, for debugging)
} sensor_avg_t;

static sensor_avg_t sensor_averages[MAX_SENSORS] = {0}; // Initialize to zero
static pthread_mutex_t avg_mutex = PTHREAD_MUTEX_INITIALIZER; // For thread safety

void *data_manager(void *arg);

#endif /* DATA_MANAGER_H */
