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
        perror("Invalid IP address");
        return;
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

    /* Handshake: Receive server's listening port */
    char buffer[BUFF_SIZE];
    if (chat_receiveMessage(sock_fd, buffer, BUFF_SIZE) <= 0)
    {
        perror("Failed to receive server port");
        close(sock_fd);
        return;
    }
    int server_port = 0;
    if (sscanf(buffer, "PORT %d", &server_port) != 1)
    {
        printf("Invalid port handshake from server\n");
        close(sock_fd);
        return;
    }

    /* Send client's listening port */
    char port_msg[32];
    snprintf(port_msg, sizeof(port_msg), "PORT %d", server_getPort());
    if (chat_sendMessage(sock_fd, port_msg) == -1)
    {
        perror("Failed to send port to server");
        close(sock_fd);
        return;
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
        perror("malloc failed");
        close(sock_fd);
        return;
    }
    data->socket_fd = sock_fd;
    data->is_server = 0;
    if (pthread_create(&chat_thread_id, NULL, chat_thread, data) != 0)
    {
        perror("chat thread failed");
        free(data);
        close(sock_fd);
        return;
    }
    pthread_detach(chat_thread_id);
}