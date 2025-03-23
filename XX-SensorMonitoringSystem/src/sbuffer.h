/** @file sbuffer.h
 *  @brief Shared mailbox declarations
 *  
 *  Declares mailbox functions/variables
 *  sbuffer means "shared sensor buffer"
 * 
 *  @author Phuc
 *  @bug No known bugs.
 */

#ifndef _SBUFFER_H
#define _SBUFFER_H

#include "../include/common.h"
#include <stdbool.h>

// Create a node for each sensor
sensorNode *createNode(sensorContent nodeContent);

// Initialize a linked list
void initList(linkedList *list);

// Search for a sensor node by its ID
sensorNode *findNode(linkedList *list, int nodeID);

// Add a sensor node to linked list
bool addNode(linkedList *list, sensorContent nodeContent);

// Remove a sensor node from the linked list
bool removeNode(linkedList *list, sensorNode *sensor);

// Remove all nodes in linked list
void removeAll(linkedList *list);

#endif /* _SBUFFER_H */
