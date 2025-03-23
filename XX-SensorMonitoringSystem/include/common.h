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

#include <pthread.h>

// Sensor data group
struct sensorContent
{
    int nodeID;  // ID of a node
    int data;    // Data provided by node
    int time;    // Real-time that node sends data
};
typedef struct sensorContent sensorContent;

// Linked list node
struct sensorNode
{
    sensorContent content;     // The data group
    struct sensorNode *next;   // Pointer to next node
};
typedef struct sensorNode sensorNode;

// Define a linked list structure
// Manages a list of sensor nodes for the shared buffer
typedef struct linkedList
{
    sensorNode *head;
    sensorNode *tail;
    pthread_mutex_t lock;
} linkedList;


#endif /* _COMMON_H */
