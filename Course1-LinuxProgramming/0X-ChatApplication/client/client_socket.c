/** @file client_socket.c
 *  @brief A socket client driver.
 *
 *  This file contains function definitions for
 *  socket client.
 *
 *  @author Phuc
 *  @bug No known bugs.
 */

#include "../include/chat.h"

extern ClientInfo clients[MAX_CLIENTS];
extern int client_count;
extern pthread_mutex_t client_mutex;

void client_socketStart(const char *server_ip, int portNum)
{
    int sock_fd;
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));

    /* Initialize client address */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portNum);
    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0)
    {
        handle_error("Invalid IP address");
    }

    /* Create socket */
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1)
    {
        handle_error("client_socket()");
    }

    /* Connect to server */
    if (connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        handle_error("client_connect()");
    }
    printf("Connected to %s:%d\n", server_ip, portNum);

    /* Receive server's listening port */
    char buffer[BUFF_SIZE];
    if (chat_receiveMessage(sock_fd, buffer, BUFF_SIZE) <= 0)
    {
        close(sock_fd);
        handle_error("Failed to receive server port");
    }
    int server_port = 0;
    /* Extract received data from server, store it in server_port */
    if (sscanf(buffer, "PORT %d", &server_port) != 1)
    {
        close(sock_fd);
        handle_error("Invalid port handshake from server\n");
    }

    /* Send client's listening port */
    char port_msg[32];
    /* Prepare data with format "PORT %d" and send to server */
    snprintf(port_msg, sizeof(port_msg), "PORT %d", server_getPort());
    if (chat_sendMessage(sock_fd, port_msg) == -1)
    {
        close(sock_fd);
        handle_error("Failed to send port to server");
    }

    /* Add to clients list */
    pthread_mutex_lock(&client_mutex);
    if (client_count < MAX_CLIENTS)
    {
        clients[client_count].socket_fd = sock_fd;
        strcpy(clients[client_count].ip_addr, server_ip);
        clients[client_count].listening_port = server_port; // Store server's listening port
        client_count++;
    }
    else
    {
        printf("Max clients reached\n");
        close(sock_fd);
        pthread_mutex_unlock(&client_mutex);
        return;
    }
    pthread_mutex_unlock(&client_mutex);

    /* Start client thread */
    pthread_t chat_thread_id;
    ClientData *data = malloc(sizeof(ClientData));
    if (!data)
    {
        close(sock_fd);
        handle_error("malloc failed");
    }
    data->socket_fd = sock_fd;
    /* Create a connection as a client, so is_server is 0 */
    data->is_server = 0;
    if (pthread_create(&chat_thread_id, NULL, chat_thread, data) != 0)
    {
        free(data);
        close(sock_fd);
        handle_error("chat thread failed");
    }
    pthread_detach(chat_thread_id);
}