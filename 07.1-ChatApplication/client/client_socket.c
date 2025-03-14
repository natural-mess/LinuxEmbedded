/** @file client_socket.c
 *  @brief A socket client driver.
 *
 *  This file contains function definitions for
 *  socket client.
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
void client_socketStart(int portNum, char* ipAddr)
{
    int server_fd;
    struct sockaddr_in serv_addr;
	memset(&serv_addr, '0',sizeof(serv_addr));

    /* Initialize client address */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port   = htons(portNum);
    if (inet_pton(AF_INET, ipAddr, &serv_addr.sin_addr) == -1) 
    {
        handle_error("client_inet_pton()");
    }
	
    /* Create socket */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        handle_error("client_socket()");
    }
	
    /* Connect to server server*/
    if (connect(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        handle_error("client_connect()");
    }
}

