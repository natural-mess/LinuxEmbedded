/** @file sensor_node.h
 *  @brief Simulates sensor nodes
 *
 *  Declare the sensor node simulation function 
 *  and any necessary types.
 *
 *  @author Phuc
 *  @bug No known bugs.
 */

#ifndef SENSOR_NODE_H
#define SENSOR_NODE_H

#include <sys/socket.h>
#include <netinet/in.h>
#include "../include/common.h"

void sensor_node_simulate(int port, int sensor_id, float temp, int send_count);

#endif /* SENSOR_NODE_H */