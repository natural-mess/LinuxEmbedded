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
static void server_addr_init(struct sockaddr_in *serv_addr)
{
    memset(serv_addr, 0, sizeof(struct sockaddr_in));
}

static void cleanup_server_fd(void)
{
    if (server_fd != -1)
    {
        close(server_fd);
        server_fd = -1;
    }
}

unsigned long server_getServerAddr(void)
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    if (getsockname(server_fd, (struct sockaddr*)&addr, &len) == -1)
    {
        perror("getsockname failed");
        return INADDR_ANY;  // Default fallback
    }
    return addr.sin_addr.s_addr;
}

unsigned short server_getPort(void)
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    if (getsockname(server_fd, (struct sockaddr*)&addr, &len) == -1)
    {
        perror("getsockname failed");
        return 0;  // Default fallback
    }
    return ntohs(addr.sin_port);
}

void server_startClientThread(int client_fd, struct sockaddr_in client_addr)
{
    /* Send our listening port */
    char port_msg[32];
    snprintf(port_msg, sizeof(port_msg), "PORT %d", server_getPort());
    if (chat_sendMessage(client_fd, port_msg) == -1)
    {
        perror("Failed to send port to client");
        close(client_fd);
        return;
    }

    /* Receive client's listening port */
    char buffer[BUFF_SIZE];
    if (chat_receiveMessage(client_fd, buffer, BUFF_SIZE) <= 0)
    {
        perror("Failed to receive client port");
        close(client_fd);
        return;
    }
    int client_port = 0;
    if (sscanf(buffer, "PORT %d", &client_port) != 1)
    {
        printf("Invalid port handshake from client\n");
        close(client_fd);
        return;
    }

    /* Add client to list */
    pthread_mutex_lock(&client_mutex);
    if (client_count >= MAX_CLIENTS)
    {
        printf("Max clients reached\n");
        close(client_fd);
        pthread_mutex_unlock(&client_mutex);
        return;
    }
    clients[client_count].socket_fd = client_fd;
    inet_ntop(AF_INET, &client_addr.sin_addr, clients[client_count].ip_addr, INET_ADDRSTRLEN);
    clients[client_count].listening_port = client_port;  // Store received port
    client_count++;
    pthread_mutex_unlock(&client_mutex);

    /* Start client thread */
    pthread_t thread;
    ClientData *data = malloc(sizeof(ClientData));
    if (!data)
    {
        perror("malloc failed");
        pthread_mutex_lock(&client_mutex);
        client_count--;
        pthread_mutex_unlock(&client_mutex);
        close(client_fd);
        return;
    }
    data->socket_fd = client_fd;
    data->is_server = 1;

    if (pthread_create(&thread, NULL, chat_thread, data) != 0)
    {
        perror("Thread creation failed");
        pthread_mutex_lock(&client_mutex);
        client_count--;
        pthread_mutex_unlock(&client_mutex);
        free(data);
        close(client_fd);
        return;
    }
    pthread_detach(thread);
    printf("\nNew connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), client_port);
}

void *connection_thread(void *arg)
{
    serverData *data = (serverData *)arg;
    int server_fd = data->server_fd;
    int port_num = data->portNum;
    free(data);
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    printf("Application is listening on port %d\n", port_num);
    while (1)
    {
        int new_socket_fd = accept(server_fd, (struct sockaddr*)&client_addr, &len);
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
    server_addr_init(&serv_addr);
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

    // Start connection thread
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

    // Existing code...
    atexit(cleanup_server_fd);  // Register cleanup
}


