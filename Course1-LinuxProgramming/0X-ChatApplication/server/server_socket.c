/** @file server_socket.c
 *  @brief A socket server driver.
 *
 *  This file contains function definitions for
 *  socket server.
 *
 *  @author Phuc
 *  @bug No know bugs.
 */

/***********
 * Include *
 ***********/
#include "../include/chat.h"

/*********************
 * Globale variables *
 *********************/
extern ClientInfo clients[MAX_CLIENTS];
extern int client_count;
extern pthread_mutex_t client_mutex;
extern int server_fd;

typedef struct {
    int server_fd;
    int portNum;
} serverData;

/***********************
 * Function definition *
 ***********************/
unsigned long server_getServerAddr(void)
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    /* Retrieves the local address (IP and port) bound to a socket */
    if (getsockname(server_fd, (struct sockaddr*)&addr, &len) == -1)
    {
        perror("getsockname failed");
        return INADDR_ANY;
    }
    return addr.sin_addr.s_addr;
}

unsigned short server_getPort(void)
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    /* Retrieves the local address (IP and port) bound to a socket */
    if (getsockname(server_fd, (struct sockaddr*)&addr, &len) == -1)
    {
        perror("getsockname failed");
        return 0;
    }
    /* network-to-host short */
    return ntohs(addr.sin_port);
}

static void server_startClientThread(int client_fd, struct sockaddr_in client_addr)
{
    /* Send our listening port */
    /* This is to tell the client the server’s listening port, 
    which the client will store for its list command. */
    char port_msg[32];
    /* Write data of format "PORT %d" to port_msg, if data is too long, takes exactly 32 characters */
    snprintf(port_msg, sizeof(port_msg), "PORT %d", server_getPort());
    if (chat_sendMessage(client_fd, port_msg) == -1)
    {
        close(client_fd);
        handle_error("Failed to send port to client");
    }

    /* Receive client's listening port */
    /* Expects the client to send its own listening port */
    char buffer[BUFF_SIZE];
    if (chat_receiveMessage(client_fd, buffer, BUFF_SIZE) <= 0)
    {
        close(client_fd);
        handle_error("Failed to receive client port");
    }
    int client_port = 0;
    /* Ensures the client sent a valid port number in the expected format "PORT %d" */
    /* Extracts port number at %d, stores it to client_port */
    /* If it’s not 1, the format is invalid */
    if (sscanf(buffer, "PORT %d", &client_port) != 1)
    {
        close(client_fd);
        handle_error("Invalid port handshake from client\n");
    }

    /* Add client to list */
    pthread_mutex_lock(&client_mutex);
    /* If full, notifies the user, closes the socket, unlocks the mutex, and exits. */
    if (client_count >= MAX_CLIENTS)
    {
        printf("Max clients reached\n");
        close(client_fd);
        pthread_mutex_unlock(&client_mutex);
        return;
    }
    clients[client_count].socket_fd = client_fd;
    /* Converts the client’s binary IP address from client_addr.sin_addr to a string and stores it. */
    inet_ntop(AF_INET, &client_addr.sin_addr, clients[client_count].ip_addr, INET_ADDRSTRLEN);
    /* Store received port */
    clients[client_count].listening_port = client_port;
    client_count++;
    pthread_mutex_unlock(&client_mutex);

    /* Start client thread */
    pthread_t thread;
    ClientData *data = malloc(sizeof(ClientData));
    if (!data)
    {
        pthread_mutex_lock(&client_mutex);
        client_count--;
        pthread_mutex_unlock(&client_mutex);
        close(client_fd);
        handle_error("malloc failed");
    }
    data->socket_fd = client_fd;
    /* Create a connection as a server, so is_server is 1 */
    data->is_server = 1;

    if (pthread_create(&thread, NULL, chat_thread, data) != 0)
    {
        pthread_mutex_lock(&client_mutex);
        client_count--;
        pthread_mutex_unlock(&client_mutex);
        free(data);
        close(client_fd);
        handle_error("Thread creation failed");
    }
    pthread_detach(thread);
    printf("\nNew connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), client_port);
}

void *connection_thread(void *arg)
{
    serverData *data = (serverData *)arg;
    int listen_fd = data->server_fd;
    int port_num = data->portNum;
    free(data);
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    printf("Application is listening on port %d\n", port_num);
    while (1)
    {
        int new_socket_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &len);
        if (new_socket_fd == -1)
        {
            perror("Accept failed");
            continue;
        }
        server_startClientThread(new_socket_fd, client_addr);
    }
    return NULL;
}

void server_socketStart(int portNum)
{
    struct sockaddr_in serv_addr;
    int opt = 1;

    /* Create socket */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        handle_error("server_socket()");
    }

    /* Set socket options to reuse address and port */
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1)
    {
        close(server_fd);
        handle_error("server_setsockopt()");
    }

    // Initialize server address
    memset(serv_addr, 0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portNum);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    /* Bind socket */
    if (bind(server_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
    {
        close(server_fd);
        handle_error("server_bind()");
    }

    /* Listen for connections */
    if (listen(server_fd, LISTEN_BACKLOG) == -1)
    {
        close(server_fd);
        handle_error("server_listen()");
    }

    /* Start connection thread */
    pthread_t conn_thread;
    serverData *data = malloc(sizeof(serverData));
    if (!data)
    {
        close(server_fd);
        handle_error("server thread malloc failed");
    }
    data->server_fd = server_fd;
    data->portNum = portNum;

    if (pthread_create(&conn_thread, NULL, connection_thread, data) != 0)
    {
        free(data);
        close(server_fd);
        handle_error("server pthread_create()");
    }
    pthread_detach(conn_thread);
}


