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

#define MIN_TEMP 0
#define MAX_TEMP 100
#define SLEEP_TIME 3

// Start sensor node simulation
void sensor_node_simulate(int port, int sensor_id, int send_count);

// Send data from sensor node
void sensor_send_data(int sock_fd, sensor_data_t* data);

#endif /* SENSOR_NODE_H */
