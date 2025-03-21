
/** @file sbuffer.c
 *  @brief The shared “mailbox” for sensor data
 *  
 *  Define mailbox functions/variables
 * 
 *  @author Phuc
 *  @bug No known bugs.
 */


#include "sbuffer.h"

dataList* createNode (nodeData x)
{
    dataList *p = (dataList*) malloc(sizeof(dataList));
    p->node.data = x.data;
    p->node.nodeID = x.nodeID;
    p->node.time = x.time;
    p->next = NULL;
    return p;
}

typedef struct LinkedList
{
    dataList *head;
    dataList *tail;
}LinkedList;

void init(LinkedList *list)
{
    list->head = NULL;
    list->tail = NULL;
}

void insertTail (LinkedList *list, nodeData x)
{
    dataList *p = createNode(x);
    if (list->head == NULL)
    {
        list->head = list->tail = p;
    }
    else
    {
        list->tail->next = p;
        list->tail = p;
    }
}


void removeAll (LinkedList *list)
{
    while (list->head != NULL)
    {
        dataList *cur = list->head;
        list->head = list->head->next;
        free(cur);
    }
    list->tail = NULL;
}
