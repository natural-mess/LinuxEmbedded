/** @file chat.c
 *  @brief Chat driver.
 *
 *  This file contains function definitions of 
 *  communication functionality of this chat application
 *
 *  @author Phuc
 *  @bug No know bugs.
 */

#include "../include/chat.h"

ClientInfo clients[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;

void *chat_thread(void *arg)
{
    ClientData *data = (ClientData *)arg;
    int socket_fd = data->socket_fd;
    int is_server = data->is_server;
    free(data);

    /* Start chat */
    ui_run(socket_fd, is_server);

    // Remove client from list
    pthread_mutex_lock(&client_mutex);
    for (int i = 0; i < client_count; i++)
    {
        if (clients[i].socket_fd == socket_fd)
        {
            /* Shift all later entries left */
            for (int j = i; j < client_count - 1; j++)
            {
                clients[j] = clients[j + 1];
            }
            client_count--;
            printf("\nConnection %d terminated\n", i + 1);
            break;
        }
    }
    pthread_mutex_unlock(&client_mutex);
    close(socket_fd);
    return NULL;
}

int chat_sendMessage(int socket_fd, const char *msg)
{
    int len = strlen(msg);
    int numb_write = write(socket_fd, msg, len);
    if (numb_write == -1)
    {
        perror("write()");
        return -1;
    }
    return numb_write;
}

int chat_receiveMessage(int socket_fd, char *buffer, size_t size)
{
    memset(buffer, 0, size);
    int numb_read = read(socket_fd, buffer, size - 1);
    if (numb_read <= 0)
    {
        if (numb_read == -1) perror("read()");
        return numb_read;
    }
    buffer[numb_read] = '\0';  // Null-terminate
    return numb_read;
}
