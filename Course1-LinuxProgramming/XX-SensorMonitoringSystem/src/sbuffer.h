/** @file sbuffer.h
 *  @brief Shared data structure declarations
 *
 *  Declares data structure to store sensors data
 *  Use circular buffer as data structure to handle data
 *
 *  @author Phuc
 *  @bug No known bugs.
 */

#ifndef _SBUFFER_H
#define _SBUFFER_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

typedef struct
{
    int sensor_id;
    float temperature;
    time_t timestamp;
} sensor_data_t;

typedef struct
{
    sensor_data_t *buffer;    // Array for circular buffer
    int size;                 // Maximum number of elements
    int head;                 // Index where next data will be added
    int tail;                 // Index where next data will be removed
    int count;                // Current number of elements
    pthread_mutex_t mutex;    // For thread safety
    pthread_cond_t not_full;  // Signal when buffer isn’t full
    pthread_cond_t not_empty; // Signal when buffer isn’t empty
} sbuffer_t;

// Initializes the shared data structure sbuffer
int sbuffer_init(sbuffer_t *sb, int size);

// Add a new sensor data to buffer
int sbuffer_push(sbuffer_t *sb, sensor_data_t data);

// Remove a sensor data from buffer
int sbuffer_pop(sbuffer_t *sb, sensor_data_t *data);

// Free all data element in buffer
int sbuffer_free(sbuffer_t *sb);

// Return count
int sbuffer_count(sbuffer_t *sb, int *bufferCount);

#endif /* _SBUFFER_H */