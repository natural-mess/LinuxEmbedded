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
#include "log.h"

// Initializes the shared data structure sbuffer
int sbuffer_init(sbuffer_t *sb, int size)
{
    // Check valid input
    if (sb == NULL || size <= 0)
    {
        perror("Invalid sensor buffer initialization");
        return -1;
    }

    // Allocate memory for buffer array
    sb->buffer = (sensor_data_t *)malloc(size * sizeof(sensor_data_t));
    if (sb->buffer == NULL)
    {
        perror("Memory allocation failed");
        return -1;
    }

    // Initialize buffer
    sb->size = size; // Max number of element
    sb->head = 0;    // Start of where data will be added
    sb->tail = 0;    // Start of where data will be removed
    sb->count = 0;   // Empty buffer

    // Mutex init
    if (pthread_mutex_init(&sb->mutex, NULL) != 0)
    {
        free(sb->buffer);
        perror("Init mutex failed");
        return -1;
    }

    // Mutex condition signals when the buffer is not full allowing more data to be pushed
    if (pthread_cond_init(&sb->not_full, NULL) != 0)
    {
        pthread_mutex_destroy(&sb->mutex);
        free(sb->buffer);
        perror("Init mutex condition for buffer fullness failed");
        return -1;
    }

    // Mutex condition signals when the buffer is not empty allowing more data to be popped
    if (pthread_cond_init(&sb->not_empty, NULL) != 0)
    {
        pthread_mutex_destroy(&sb->mutex);
        free(sb->buffer);
        perror("Init mutex condition for buffer emptiness failed");
        return -1;
    }

    // If everything is fine, return 0
    return 0;
}

// Add a new sensor data node to buffer
int sbuffer_push(sbuffer_t *sb, sensor_data_t data)
{
    // Check valid input
    if (sb == NULL)
    {
        perror("Invalid sensor buffer, push failed");
        return -1;
    }

    // Lock mutex
    if (pthread_mutex_lock(&sb->mutex) != 0)
    {
        perror("Mutex lock failed in push");
        return -1;
    }

    // Check if buffer is full
    if (sb->count == sb->size)
    {
        printf("Warning: Buffer full, overwriting oldest data (sensor %d)\n", data.sensor_id);
        sb->tail = (sb->tail + 1) % sb->size;
        sb->count--;

        // Log the event saying data overwritten
        char msg[256];
        snprintf(msg, sizeof(msg), "Dropped oldest data from sensor %d due to buffer full", sb->buffer[sb->tail].sensor_id);
        log_event(msg);
    }

    // Add data to the buffer at head position
    sb->buffer[sb->head] = data;
    sb->head = (sb->head + 1) % sb->size;
    sb->count++;

    // Signal that data is now available (buffer is not empty)
    if (pthread_cond_signal(&sb->not_empty) != 0)
    {
        perror("Signal not_empty failed in push");
        pthread_mutex_unlock(&sb->mutex);
        return -1;
    }

    // Unlock the mutex
    if (pthread_mutex_unlock(&sb->mutex) != 0)
    {
        perror("Mutex unlock failed in push");
        return -1;
    }

    return 0; // Success
}

// Remove a sensor data node from buffer
int sbuffer_pop(sbuffer_t *sb, sensor_data_t *data)
{
    // Check valid input
    if (sb == NULL || data == NULL)
    {
        perror("Invalid sensor buffer or data pointer, pop failed");
        return -1;
    }

    // Lock mutex
    if (pthread_mutex_lock(&sb->mutex) != 0)
    {
        perror("Mutex lock failed in pop");
        return -1;
    }

    while (sb->count == 0)
    {
        printf("Buffer is empty, waiting...\n");
        if (pthread_cond_wait(&sb->not_empty, &sb->mutex) != 0)
        {
            pthread_mutex_unlock(&sb->mutex);
            perror("Condition wait failed in pop");
            return -1;
        }
    }

    *data = sb->buffer[sb->tail];
    sb->tail = (sb->tail + 1) % sb->size;
    sb->count--;

    if (pthread_cond_signal(&sb->not_full) != 0)
    {
        perror("Signal not_full failed in pop");
        pthread_mutex_unlock(&sb->mutex);
        return -1;
    }

    // Unlock the mutex
    if (pthread_mutex_unlock(&sb->mutex) != 0)
    {
        perror("Mutex unlock failed in pop");
        return -1;
    }

    return 0; // Success
}

// Free all nodes in buffer
int sbuffer_free(sbuffer_t *sb)
{
    // Check valid input
    if (sb == NULL)
    {
        perror("Invalid sensor buffer, sbuffer_free failed");
        return -1;
    }

    // Lock mutex
    if (pthread_mutex_lock(&sb->mutex) != 0)
    {
        perror("Mutex lock failed in sbuffer_free");
        return -1;
    }

    // Free the buffer array (single allocation from sbuffer_init)
    free(sb->buffer);
    sb->buffer = NULL; // Prevent dangling pointer
    sb->size = 0;      // Clear state
    sb->head = 0;
    sb->tail = 0;
    sb->count = 0;

    // Unlock the mutex
    if (pthread_mutex_unlock(&sb->mutex) != 0)
    {
        perror("Mutex unlock failed in pop");
        return -1;
    }

    // Destroy mutex
    if (pthread_mutex_destroy(&sb->mutex) != 0)
    {
        perror("Mutex destroy failed in sbuffer_free");
        return -1;
    }
    if (pthread_cond_destroy(&sb->not_full) != 0)
    {
        perror("Not_full condition destroy failed in sbuffer_free");
        return -1;
    }
    if (pthread_cond_destroy(&sb->not_empty) != 0)
    {
        perror("Not_empty condition destroy failed in sbuffer_free");
        return -1;
    }

    return 0; // Success
}

// Return count
int sbuffer_count(sbuffer_t *sb, int *bufferCount)
{
    if (sb == NULL || bufferCount == NULL)
    {
        perror("Invalid sensor buffer or bufferCount pointer, sbuffer_count failed");
        return -1;
    }

    // Lock mutex
    if (pthread_mutex_lock(&sb->mutex) != 0)
    {
        perror("Mutex lock failed in sbuffer_count");
        return -1;
    }

    *bufferCount = sb->count;

    // Unlock the mutex
    if (pthread_mutex_unlock(&sb->mutex) != 0)
    {
        perror("Mutex unlock failed in sbuffer_count");
        return -1;
    }

    return 0; // Success
}
