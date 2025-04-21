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
#include <time.h>
#include <string.h>
#include "../include/common.h"
#include "data_manager.h"
#include "storage_manager.h"
#include "sbuffer.h"
#include "log.h"
#include "threads.h"

sensor_avg_t sensor_averages[MAX_SENSORS] = {0};
pthread_mutex_t avg_mutex = PTHREAD_MUTEX_INITIALIZER;

void *data_manager(void *arg)
{
    thread_args_t *args = (thread_args_t *)arg;
    sbuffer_t *sb = args->sb;
    char msg[256];
    // Track last alert time per sensor
    static time_t last_alert_time[MAX_SENSORS] = {0};

    while (!shutdown_flag)
    {
        sensor_data_t data;

        int pop_retries = 0;
        while (pop_retries < MAX_RETRIES && sbuffer_pop(sb, &data) != 0)
        {
            if (shutdown_flag)
                goto cleanup;
            snprintf(msg, sizeof(msg), "Failed to pop data from sbuffer, retry %d/%d", pop_retries + 1, MAX_RETRIES);
            log_event(msg);
            sleep(1);
            pop_retries++;
        }

        if (pop_retries == MAX_RETRIES)
        {
            log_event("Max retries reached for popping data, skipping...");
            continue;
        }

        // Validate sensor ID (assume valid IDs start at 1)
        if (data.sensor_id <= 0 || data.sensor_id >= MAX_SENSORS)
        {
            snprintf(msg, sizeof(msg), "Received sensor data with invalid sensor ID %d", data.sensor_id);
            log_event(msg);
            continue;
        }

        // Log raw data for debugging
        snprintf(msg, sizeof(msg), "Processing sensor %d: temp=%.1f°C, time=%ld",
                 data.sensor_id, data.temperature, data.timestamp);
        log_event(msg);

        time_t now = time(NULL);
        float new_sum, new_avg;
        int new_count;

        if (pthread_mutex_lock(&avg_mutex) != 0)
        {
            snprintf(msg, sizeof(msg), "Mutex lock failed in data_manager for sensor %d", data.sensor_id);
            log_event(msg);
            continue;
        }

        // Reset average if no recent updates
        if (difftime(now, sensor_averages[data.sensor_id].last_update) > RESET_THRESHOLD_SECONDS)
        {
            sensor_averages[data.sensor_id].sum = data.temperature;
            sensor_averages[data.sensor_id].count = 1;
            snprintf(msg, sizeof(msg), "Reset average for sensor %d to %.1f°C",
                     data.sensor_id, data.temperature);
            log_event(msg);
        }
        else
        {
            sensor_averages[data.sensor_id].sum += data.temperature;
            sensor_averages[data.sensor_id].count++;
        }

        new_sum = sensor_averages[data.sensor_id].sum;
        new_count = sensor_averages[data.sensor_id].count;
        sensor_averages[data.sensor_id].last_update = data.timestamp;

        // Only calculate average if we have enough readings
        if (new_count >= MIN_AVG_COUNT)
        {
            new_avg = new_sum / new_count;
            // Log running average for debugging
            snprintf(msg, sizeof(msg), "Sensor %d running avg: %.1f°C (count=%d)",
                     data.sensor_id, new_avg, new_count);
            log_event(msg);
        }
        else
        {
            snprintf(msg, sizeof(msg), "Sensor %d accumulating: %.1f°C (count=%d, waiting for %d)",
                     data.sensor_id, data.temperature, new_count, MIN_AVG_COUNT);
            log_event(msg);
            new_avg = 0.0; // Avoid using average until MIN_AVG_COUNT
        }

        if (pthread_mutex_unlock(&avg_mutex) != 0)
        {
            snprintf(msg, sizeof(msg), "Mutex unlock failed in data_manager for sensor %d", data.sensor_id);
            log_event(msg);
        }

        // Check temperature conditions only if we have enough readings
        if (new_count >= MIN_AVG_COUNT)
        {
            // Only alert if enough time has passed since the last alert
            if (difftime(now, last_alert_time[data.sensor_id]) >= ALERT_COOLDOWN)
            {
                if (new_avg < TOO_COLD)
                {
                    snprintf(msg, sizeof(msg), "The sensor node with %d reports it's too cold (running avg temperature = %.1f)",
                             data.sensor_id, new_avg);
                    log_event(msg);
                    time_t now_alert = time(NULL);
                    char time_str[26];
                    ctime_r(&now_alert, time_str);
                    time_str[strlen(time_str) - 1] = '\0';
                    printf("%s: Sensor %d too cold (avg temp %.1f°C)\n", time_str, data.sensor_id, new_avg);
                    last_alert_time[data.sensor_id] = now;
                }
                else if (new_avg > TOO_HOT)
                {
                    snprintf(msg, sizeof(msg), "The sensor node with %d reports it's too hot (running avg temperature = %.1f)",
                             data.sensor_id, new_avg);
                    log_event(msg);
                    time_t now_alert = time(NULL);
                    char time_str[26];
                    ctime_r(&now_alert, time_str);
                    time_str[strlen(time_str) - 1] = '\0';
                    printf("%s: Sensor %d too hot (avg temp %.1f°C)\n", time_str, data.sensor_id, new_avg);
                    last_alert_time[data.sensor_id] = now;
                }
            }
        }
    }

cleanup:
    log_event("Data manager shutting down");
    return NULL;
}