/** @file data_manager.c
 *  @brief Implementation of the data manager
 *
 *  Processes sensor data, calculates running averages, and logs temperature conditions.
 *
 *  @author Phuc
 *  @bug No known bugs.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "data_manager.h"
#include "sbuffer.h"
#include "log.h"
#include "threads.h"

void* data_manager(void* arg)
{
    thread_args_t* args = (thread_args_t*)arg;
    sbuffer_t* sb = args->sb;

    char msg[256]; // Buffer for log messages

    // Use global shutdown flag from common.h
    while (!shutdown_flag)
    {
        sensor_data_t data;

        // Pop data from sbuffer, waiting if empty
        if (sbuffer_pop(sb, &data) != 0)
        {
            snprintf(msg, sizeof(msg), "Failed to pop data from sbuffer");
            log_event(msg);
            continue; // Skip this iteration if pop fails
        }

        // Check for invalid sensor ID
        if (data.sensor_id < 0 || data.sensor_id >= MAX_SENSORS)
        {
            snprintf(msg, sizeof(msg), "Received sensor data with invalid sensor node ID %d", data.sensor_id);
            log_event(msg);
            continue; // Skip processing invalid data
        }

        // Update running average (thread-safe)
        if (pthread_mutex_lock(&avg_mutex) != 0)
        {
            snprintf(msg, sizeof(msg), "Mutex lock failed in data_manager for sensor %d", data.sensor_id);
            log_event(msg);
            continue;
        }

        // Update running average
        sensor_averages[data.sensor_id].sum += data.temperature;
        sensor_averages[data.sensor_id].count++;
        sensor_averages[data.sensor_id].last_update = data.timestamp;

        float avg = sensor_averages[data.sensor_id].sum / sensor_averages[data.sensor_id].count;

        // Check temperature conditions
        if (avg < TOO_COLD)
        { // Too cold threshold
            snprintf(msg, sizeof(msg), "The sensor node with %d reports it's too cold (running avg temperature = %.1f)", data.sensor_id, avg);
            log_event(msg);
        }
        else if (avg > TOO_HOT)
        { // Too hot threshold
            snprintf(msg, sizeof(msg), "The sensor node with %d reports it's too hot (running avg temperature = %.1f)", data.sensor_id, avg);
            log_event(msg);
        }

        if (pthread_mutex_unlock(&avg_mutex) != 0)
        {
            snprintf(msg, sizeof(msg), "Mutex unlock failed in data_manager for sensor %d", data.sensor_id);
            log_event(msg);
        }
    }

    return NULL;
}
