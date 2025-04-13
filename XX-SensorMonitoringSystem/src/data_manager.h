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

// Define configurable thresholds (could be passed via config)
#define TOO_HOT 40.0
#define TOO_COLD 18.0
#define RESET_THRESHOLD_SECONDS 3600 // Reset average after 1 hour of inactivity

// Minimum readings required before averaging and alerting
#define MIN_AVG_COUNT 5
// Minimum time between alerts for the same sensor (seconds)
#define ALERT_COOLDOWN 60

typedef struct
{
    float sum;        // Sum of temperatures for running average
    int count;        // Number of readings
    time_t last_update; // Last time updated (optional, for debugging)
} sensor_avg_t;

// Global variables (make extern if needed in other files)
extern sensor_avg_t sensor_averages[MAX_SENSORS];
extern pthread_mutex_t avg_mutex;

void *data_manager(void *arg);

#endif /* DATA_MANAGER_H */
