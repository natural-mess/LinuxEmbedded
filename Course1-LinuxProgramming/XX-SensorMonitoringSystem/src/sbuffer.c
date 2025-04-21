/** @file sbuffer.c
 *  @brief Shared data structure declarations
 *
 *  Declares data structure to store sensors data
 *  Use circular buffer as data structure to handle data
 *
 *  @author Phuc
 *  @bug No known bugs.
 */

#include "sbuffer.h"
#include <error.h>
#include <unistd.h>
#include "log.h"
#include "../include/common.h"

// Initializes the shared data structure sbuffer
int sbuffer_init(sbuffer_t *sb, int size)
{
    if (sb == NULL || size <= 0)
    {
        perror("Invalid sensor buffer initialization");
        return -1;
    }

    sb->buffer = (sensor_data_t *)malloc(size * sizeof(sensor_data_t));
    if (sb->buffer == NULL)
    {
        perror("Memory allocation failed");
        return -1;
    }

    sb->size = size;
    sb->head = 0;
    sb->tail = 0;
    sb->count = 0;

    if (pthread_mutex_init(&sb->mutex, NULL) != 0)
    {
        free(sb->buffer);
        perror("Init mutex failed");
        return -1;
    }

    if (pthread_cond_init(&sb->not_full, NULL) != 0)
    {
        pthread_mutex_destroy(&sb->mutex);
        free(sb->buffer);
        perror("Init mutex condition for buffer fullness failed");
        return -1;
    }

    if (pthread_cond_init(&sb->not_empty, NULL) != 0)
    {
        pthread_mutex_destroy(&sb->mutex);
        free(sb->buffer);
        perror("Init mutex condition for buffer emptiness failed");
        return -1;
    }

    return 0;
}

// Add a new sensor data node to buffer
int sbuffer_push(sbuffer_t *sb, sensor_data_t data)
{
    if (sb == NULL)
    {
        perror("Invalid sensor buffer, push failed");
        return -1;
    }

    if (pthread_mutex_lock(&sb->mutex) != 0)
    {
        perror("Mutex lock failed in push");
        return -1;
    }

    if (sb->count == sb->size)
    {
        printf("Warning: Buffer full, overwriting oldest data (sensor %d)\n", data.sensor_id);
        sb->tail = (sb->tail + 1) % sb->size;
        sb->count--;

        char msg[256];
        snprintf(msg, sizeof(msg), "Dropped oldest data from sensor %d due to buffer full", sb->buffer[sb->tail].sensor_id);
        log_event(msg);
    }

    sb->buffer[sb->head] = data;
    sb->head = (sb->head + 1) % sb->size;
    sb->count++;

    log_event("Data pushed to buffer");

    if (pthread_cond_signal(&sb->not_empty) != 0)
    {
        perror("Signal not_empty failed in push");
        pthread_mutex_unlock(&sb->mutex);
        return -1;
    }

    if (pthread_mutex_unlock(&sb->mutex) != 0)
    {
        perror("Mutex unlock failed in push");
        return -1;
    }

    return 0;
}

// Remove a sensor data node from buffer
int sbuffer_pop(sbuffer_t *sb, sensor_data_t *data)
{
    if (sb == NULL || data == NULL)
    {
        perror("Invalid sensor buffer or data pointer, pop failed");
        return -1;
    }

    if (pthread_mutex_lock(&sb->mutex) != 0)
    {
        perror("Mutex lock failed in pop");
        return -1;
    }

    while (sb->count == 0 && !shutdown_flag)
    {
        printf("Buffer is empty, waiting...\n");
        if (pthread_cond_wait(&sb->not_empty, &sb->mutex) != 0)
        {
            pthread_mutex_unlock(&sb->mutex);
            perror("Condition wait failed in pop");
            return -1;
        }
    }

    if (sb->count == 0)
    {
        pthread_mutex_unlock(&sb->mutex);
        return -1; // Exit if buffer is empty (including during shutdown)
    }

    *data = sb->buffer[sb->tail];
    sb->tail = (sb->tail + 1) % sb->size;
    sb->count--;

    log_event("Data popped from buffer");

    if (pthread_cond_signal(&sb->not_full) != 0)
    {
        perror("Signal not_full failed in pop");
        pthread_mutex_unlock(&sb->mutex);
        return -1;
    }

    if (pthread_mutex_unlock(&sb->mutex) != 0)
    {
        perror("Mutex unlock failed in pop");
        return -1;
    }

    return 0;
}

// Free all nodes in buffer
int sbuffer_free(sbuffer_t *sb)
{
    if (sb == NULL)
    {
        perror("Invalid sensor buffer, sbuffer_free failed");
        return -1;
    }

    int lock_retries = 0;
    while (lock_retries < MAX_RETRIES)
    {
        if (pthread_mutex_lock(&sb->mutex) == 0)
            break;
        perror("Mutex lock failed in sbuffer_free");
        usleep(100000);
        lock_retries++;
    }

    if (lock_retries == MAX_RETRIES)
    {
        log_event("Failed to lock mutex in sbuffer_free after retries");
        return -1;
    }

    free(sb->buffer);
    sb->buffer = NULL;
    sb->size = 0;
    sb->head = 0;
    sb->tail = 0;
    sb->count = 0;

    if (pthread_mutex_unlock(&sb->mutex) != 0)
    {
        log_event("Mutex unlock failed in sbuffer_free");
    }

    int destroy_retries = 0;
    while (destroy_retries < MAX_RETRIES)
    {
        if (pthread_mutex_destroy(&sb->mutex) == 0)
            break;
        perror("Mutex destroy failed in sbuffer_free");
        log_event("Mutex destroy failed in sbuffer_free");
        usleep(100000);
        destroy_retries++;
    }

    if (destroy_retries == MAX_RETRIES)
    {
        log_event("Failed to destroy mutex in sbuffer_free after retries");
        return -1;
    }

    if (pthread_cond_destroy(&sb->not_full) != 0)
    {
        perror("Not_full condition destroy failed in sbuffer_free");
        log_event("Not_full condition destroy failed in sbuffer_free");
        return -1;
    }

    if (pthread_cond_destroy(&sb->not_empty) != 0)
    {
        perror("Not_empty condition destroy failed in sbuffer_free");
        log_event("Not_empty condition destroy failed in sbuffer_free");
        return -1;
    }

    return 0;
}

// Return count
int sbuffer_count(sbuffer_t *sb, int *bufferCount)
{
    if (sb == NULL || bufferCount == NULL)
    {
        perror("Invalid sensor buffer or bufferCount pointer, sbuffer_count failed");
        return -1;
    }

    if (pthread_mutex_lock(&sb->mutex) != 0)
    {
        perror("Mutex lock failed in sbuffer_count");
        return -1;
    }

    *bufferCount = sb->count;

    if (pthread_mutex_unlock(&sb->mutex) != 0)
    {
        perror("Mutex unlock failed in sbuffer_count");
        return -1;
    }

    return 0;
}