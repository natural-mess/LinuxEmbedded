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
#include <unistd.h>
#include "data_manager.h"
#include "storage_manager.h"
#include "sbuffer.h"
#include "log.h"
#include "threads.h"

sensor_avg_t sensor_averages[MAX_SENSORS] = {0}; // Initialize to zero
pthread_mutex_t avg_mutex = PTHREAD_MUTEX_INITIALIZER;

void *data_manager(void *arg)
{
    thread_args_t *args = (thread_args_t *)arg;
    sbuffer_t *sb = args->sb;
    char msg[256];

    // Main loop
    while (!shutdown_flag)
    {
        sensor_data_t data;

        // Check for shutdown before blocking on sbuffer_pop
        if (shutdown_flag)
            break;

        // Pop data from sbuffer with retry logic
        int pop_retries = 0;
        while (pop_retries < MAX_RETRIES && sbuffer_pop(sb, &data) != 0)
        {
            snprintf(msg, sizeof(msg), "Failed to pop data from sbuffer, retry %d/%d", pop_retries + 1, MAX_RETRIES);
            log_event(msg);
            sleep(1); // Brief pause before retry
            pop_retries++;
        }

        if (pop_retries == MAX_RETRIES)
        {
            log_event("Max retries reached for popping data, skipping...");
            continue;
        }

        // Validate sensor ID
        if (data.sensor_id < 0 || data.sensor_id >= MAX_SENSORS)
        {
            snprintf(msg, sizeof(msg), "Received sensor data with invalid sensor node ID %d", data.sensor_id);
            log_event(msg);
            continue;
        }

        // Calculate new average locally to minimize lock time
        time_t now = time(NULL);
        float new_sum, new_avg;
        int new_count;

        if (pthread_mutex_lock(&avg_mutex) != 0)
        {
            snprintf(msg, sizeof(msg), "Mutex lock failed in data_manager for sensor %d", data.sensor_id);
            log_event(msg);
            continue;
        }

        // Reset if no recent updates
        if (difftime(now, sensor_averages[data.sensor_id].last_update) > RESET_THRESHOLD_SECONDS)
        {
            sensor_averages[data.sensor_id].sum = data.temperature;
            sensor_averages[data.sensor_id].count = 1;
        }
        else
        {
            sensor_averages[data.sensor_id].sum += data.temperature;
            sensor_averages[data.sensor_id].count++;
        }

        new_sum = sensor_averages[data.sensor_id].sum;
        new_count = sensor_averages[data.sensor_id].count;
        sensor_averages[data.sensor_id].last_update = data.timestamp;

        new_avg = new_sum / new_count;

        if (pthread_mutex_unlock(&avg_mutex) != 0)
        {
            snprintf(msg, sizeof(msg), "Mutex unlock failed in data_manager for sensor %d", data.sensor_id);
            log_event(msg);
        }

        // Check temperature conditions outside lock
        if (new_avg < TOO_COLD)
        {
            snprintf(msg, sizeof(msg), "The sensor node with %d reports it's too cold (running avg temperature = %.1f)", data.sensor_id, new_avg);
            log_event(msg);
        }
        else if (new_avg > TOO_HOT)
        {
            snprintf(msg, sizeof(msg), "The sensor node with %d reports it's too hot (running avg temperature = %.1f)", data.sensor_id, new_avg);
            log_event(msg);
        }
    }

    log_event("Data manager shutting down");
    return NULL;
}
