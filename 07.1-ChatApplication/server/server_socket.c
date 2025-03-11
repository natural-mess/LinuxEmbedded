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

/***********************
 * Function definition *
 ***********************/
void server_socketStart(int portNum)
{
    int server_fd, new_socket_fd;
    int len, opt;
    struct sockaddr_in serv_addr, client_addr;

    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    memset(&client_addr, 0, sizeof(struct sockaddr_in));

    /* Create socket */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        handle_error("socket()");
    }

    /* Prevent error : “address already in use” */
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        handle_error("setsockopt()");
    }

    /* Initialize server's address */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_no);
    serv_addr.sin_addr.s_addr = INADDR_ANY; // INADDR_ANY
    /* Tie socket to the server address */
    if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        handle_error("bind()");
    }

    /* Listen to maximum number of possible connections in queue */
    if (listen(server_fd, SOMAXCONN) == -1)
    {
        handle_error("listen()");
    }

    len = sizeof(client_addr);

    while (1)
    {
        printf("Server is listening at port : %d \n....\n", port_no);
        new_socket_fd = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&len);
        if (new_socket_fd == -1)
        {
            handle_error("accept()");
        }

        printf("Server : got connection \n");
    }
    close(server_fd);
}
