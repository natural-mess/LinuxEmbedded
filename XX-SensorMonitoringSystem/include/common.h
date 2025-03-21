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

#include <stdio.h>
#include <stdlib.h>

/* Node format */
typedef struct 
{
    int nodeID, data, time;
}nodeData;

/* Linked list to store data */
typedef struct dataList
{
    nodeData node;
    dataList *next;
}dataList;

#endif /* _COMMON_H */
