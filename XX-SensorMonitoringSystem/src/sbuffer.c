/** @file sbuffer.c
 *  @brief The shared “mailbox” for sensor data
 *
 *  Define mailbox functions/variables
 *  sbuffer means "shared sensor buffer"
 *
 *  @author Phuc
 *  @bug No known bugs.
 */

#include "sbuffer.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>

// Create a node for each sensor
sensorNode *createNode(sensorContent nodeContent)
{
    sensorNode *sensor = malloc(sizeof(sensorNode));
    if (!sensor)
    {
        perror("Failed to create a new sensor node");
        return NULL;
    }
    sensor->content.data = nodeContent.data;
    sensor->content.nodeID = nodeContent.nodeID;
    sensor->content.time = nodeContent.time;
    sensor->next = NULL;
    return sensor;
}

// Initialize a linked list
void initList(linkedList *list)
{
    list->head = NULL;
    list->tail = NULL;
    pthread_mutex_init(&list->lock, NULL);
}

// Search for a sensor node by its ID
sensorNode *findNode(linkedList *list, int nodeID)
{
    sensorNode *cur = list->head;

    // Search for sensor's ID in the node
    while(cur != NULL && cur->content.nodeID != nodeID)
    {
        cur = cur->next;
    }

    // Returns node or NULL
    return cur;
}

// Add a sensor node to linked list
bool addNode(linkedList *list, sensorContent nodeContent)
{
    pthread_mutex_lock(&list->lock);
    // Check if node exists already
    if (findNode(list, nodeContent.nodeID) != NULL)
    {
        fprintf(stderr, "Node %d exists, unable to add\n", nodeContent.nodeID);
        pthread_mutex_unlock(&list->lock);
        return false;
    }

    // If note doesn't exist, create one
    sensorNode *p = createNode(nodeContent);
    if (p == NULL)
    {
        perror("Failed to create a new sensor node in addNode");
        pthread_mutex_unlock(&list->lock);
        return false;
    }

    // Start adding node
    if (list->head == NULL)
    {
        list->head = list->tail = p;
    }
    else
    {
        list->tail->next = p;
        list->tail = p;
    }
    pthread_mutex_unlock(&list->lock);

    return true;
}

// Remove a sensor node from the linked list
bool removeNode(linkedList *list, sensorNode *sensor)
{
    pthread_mutex_lock(&list->lock);
    int nodeID = sensor->content.nodeID;
    sensorNode *prevNode = NULL, *cur = list->head;

    // Search and remove in one pass
    while (cur != NULL && cur->content.nodeID != nodeID)
    {
        prevNode = cur;
        cur = cur->next;
    }

    // If not found, exit
    if (cur == NULL)
    {
        pthread_mutex_unlock(&list->lock);
        return false;
    }

    // Unlink node
    if (prevNode == NULL)  // Head case
    {
        list->head = cur->next;
    }
    else
    {
        prevNode->next = cur->next;
    }

    // Update tail if needed
    if (cur == list->tail)
    {
        list->tail = prevNode;
    }

    free(cur);
    pthread_mutex_unlock(&list->lock);
    return true;
}

// Remove all nodes in linked list
void removeAll(linkedList *list)
{
    pthread_mutex_lock(&list->lock);
    while (list->head != NULL)
    {
        sensorNode *cur = list->head;
        list->head = list->head->next;
        free(cur);
    }
    list->tail = NULL;
    pthread_mutex_unlock(&list->lock);
}
