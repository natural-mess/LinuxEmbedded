/** @file sensor_node.c
 *  @brief Simulates sensor nodes
 *
 *  Set up the main function to parse arguments
 *  and run the simulation.
 *
 *  @author Phuc
 *  @bug No known bugs.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <arpa/inet.h>
#include <ctype.h>
#include "sensor_node.h"
#include "sbuffer.h"

static volatile int should_exit = 0;

static void signal_handler(int sig)
{
    should_exit = 1;
    printf("Sensor node received shutdown signal\n");
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    int portNum, sensor_id, send_count = -1;

    if (argc >= 3)
    {
        for (int i = 0; argv[1][i]; i++)
        {
            if (!isdigit(argv[1][i]))
            {
                printf("Usage: ./sensor_node <port> <sensor_id> [send_count]\n");
                exit(EXIT_FAILURE);
            }
        }

        portNum = atoi(argv[1]);
        if (portNum < 1024 || portNum > 65535)
        {
            perror("Invalid port number");
            exit(EXIT_FAILURE);
        }

        sensor_id = atoi(argv[2]);
        if (sensor_id < 0)
        {
            perror("Invalid sensor id");
            exit(EXIT_FAILURE);
        }

        send_count = atoi(argv[3]);
        if (send_count < -1)
        {
            perror("Invalid send counter");
            exit(EXIT_FAILURE);
        }

        // Create socket connection to gateway
        sensor_node_simulate(portNum, sensor_id, send_count);
    }
    else
    {
        printf("Usage: ./sensor_node <port> <sensor_id> [send_count]\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}

// Send data from sensor node
void sensor_send_data(int sock_fd, sensor_data_t *data)
{
    // Update temperature each time
    data->temperature = MIN_TEMP + (rand() % (int)(MAX_TEMP - MIN_TEMP + 1)) + (rand() / (RAND_MAX + 1.0));
    data->timestamp = time(NULL);

    // Send data
    int send_byte = write(sock_fd, data, sizeof(sensor_data_t));
    if (send_byte == sizeof(sensor_data_t))
    {
        // Sleep 5s
        sleep(SLEEP_TIME);

        char time_str[26];
        ctime_r(&(data->timestamp), time_str);
        time_str[strlen(time_str) - 1] = '\0';
        printf("Sent data: sensor_id=%d, temp=%f, time=%s \n", data->sensor_id, data->temperature, time_str);
    }
    else
    {
        perror("Failed to send data");
        exit(EXIT_FAILURE);
    }
}

// Start sensor node simulation
void sensor_node_simulate(int port, int sensor_id, int send_count)
{
    // Create socket
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1)
    {
        perror("Failed to create socket in sensor_node_simulate");
        exit(EXIT_FAILURE);
    }

    // Initialize client address
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Failed to connect to gateway");
        exit(EXIT_FAILURE);
    }

    // Confirm connection success
    printf("Connected to gateway on port %d with sensor_id %d\n", port, sensor_id);

    // Prepare data to send to gateway
    sensor_data_t data;
    data.sensor_id = sensor_id;
    data.temperature = 0;
    data.timestamp = time(NULL);

    // Seed random
    srand(time(NULL));

    // Register signal
    if (signal(SIGINT, signal_handler) == SIG_ERR)
    {
        perror("Cannot handle SIGINT");
        exit(EXIT_FAILURE);
    }

    if (send_count == -1)
    {
        // If send_count is -1, send infinitely
        while (!should_exit)
        {
            sensor_send_data(sock_fd, &data);
        }
    }
    else
    {
        for (int i = 0; i < send_count; i++)
        {
            sensor_send_data(sock_fd, &data);
        }
    }
    close(sock_fd);
    printf("Sensor %d disconnected after %d sends\n", data.sensor_id, send_count);
}