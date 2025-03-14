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
struct sockaddr_in serv_addr, client_addr;

/***********************
 * Function definition *
 ***********************/
static void server_addrInit(void)
{
    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    memset(&client_addr, 0, sizeof(struct sockaddr_in));
}

unsigned long server_getServerAddr(void)
{
    return serv_addr.sin_addr.s_addr;
}

unsigned short server_getPort(void)
{
    return ntohs(serv_addr.sin_port);
}

void server_socketStart(int portNum)
{
    int server_fd, new_socket_fd;
    int len, opt;

    /* Initialize serv_addr and client_addr to 0 */
    server_addrInit();

    /* Create socket */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        handle_error("server_socket()");
    }

    /* Prevent error : “address already in use” */
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        handle_error("server_setsockopt()");
    }

    /* Initialize server's address */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portNum);
    serv_addr.sin_addr.s_addr = INADDR_ANY; // INADDR_ANY
    /* Tie socket to the server address */
    if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        handle_error("server_bind()");
    }

    /* Listen to maximum number of possible connections in queue */
    if (listen(server_fd, SOMAXCONN) == -1)
    {
        handle_error("server_listen()");
    }

    len = sizeof(client_addr);

    while (1)
    {
        printf("Server is listening at port : %d \n....\n", portNum);
        new_socket_fd = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&len);
        if (new_socket_fd == -1)
        {
            handle_error("server_accept()");
        }

        printf("Server : got connection \n");
    }
    close(server_fd);
}


