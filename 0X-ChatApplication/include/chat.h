/** @file chat.h
 *  @brief Function prototypes for the chat application driver.
 *
 *  This contains the prototypes for the chat application server
 *  driver and eventually any macros, constants,
 *  or global variables you will need.
 *
 *  @author Phuc
 *  @bug No known bugs.
 */


#ifndef _CHAT_H
#define _CHAT_H

/***********
 * Include *
 ***********/ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

/**********
 * Define *
 **********/
/* List of commands */
#define cmd_help        "help"
#define cmd_myip        "myip"
#define cmd_myport      "myport"
#define cmd_connect     "connect"
#define cmd_list        "list"
#define cmd_terminate   "terminate"
#define cmd_send        "send"
#define cmd_exit        "exit"

/* Number of queued connection */
#define LISTEN_BACKLOG  50
#define MAX_CLIENTS     10
#define BUFF_SIZE       256
#define MSG_SIZE        100
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)
 
/********************
 * Globle variables *
 ********************/
/* Client info structure */
typedef struct
{
    int socket_fd;
    char ip_addr[INET_ADDRSTRLEN];
    int listening_port;
} ClientInfo;

typedef struct
{
    int socket_fd;
    int is_server;
} ClientData;

/**********************
 * Function prototype *
 **********************/
/* 
 * UI drivers 
 */
/** @brief Show list of commands and instructions
 *
 *  Chat application introduction and instructions
 *
 *  @param none
 *  @return Void.
 */
void ui_start(void);

/** @brief List all connected clients
 *
 *  If no client connected, print "No clients connected"
 *
 *  @param none
 *  @return Void.
 */
void ui_listClientConnection(void);

/** @brief Command handler
 *
 *  Base on user input, this function will handle each input.
 *
 *  @param none
 *  @return Void.
 */
void *ui_commandHandler(void *arg);

/** @brief Start User Interface of app
 *
 *
 *  @param socket_fd
 *  @param is_server
 *  @return Void.
 */
void ui_run(int socket_fd, int is_server);

/*
 * Server drivers
 */
/** @brief Get server IP address
 *
 *  Return server's IP address if requested by user
 *
 *  @param none
 *  @return unsigned long.
 */
unsigned long server_getServerAddr(void);

/** @brief Get server port
 *
 *  Return server's port if requested by user
 *
 *  @param none
 *  @return unsigned short.
 */
unsigned short server_getPort(void);

/** @brief Start a socket server on provided port number
 *
 *  If the port number is not valid, an error message will be printed, 
 *  new port number should be given.
 *
 *  @param portNum Port number to start socket server on.
 *  @return Void.
 */
void server_socketStart(int portNum);

/*
 * Client drivers
 */
/** @brief Start a socket client on provided port number
 *
 *  If the port number is not valid, an error message will be printed, 
 *  new port number should be given.
 *
 *  @param portNum Port number to start socket client on.
 *  @return Void.
 */
void client_socketStart(const char *server_ip, int portNum);

/*
 * Chat drivers
 */
/** @brief Send message from one machine to another
 *
 *  Content of message and socket fd are required
 *
 *  @param socket_fd Socket handler that user wants to write to
 *  @param mgs       Message content
 *  @return int: number of bytes sent
 */
int chat_sendMessage(int socket_fd, const char *msg);

/** @brief Receive message from a machine
 *
 *  Content of message, size of messge and socket fd are required
 *
 *  @param socket_fd Socket handler that user wants to write to
 *  @param buffer    Message buffer
 *  @param size      Size of message
 *  @return int: number of bytes read
 */
int chat_receiveMessage(int socket_fd, char *buffer, size_t size);

/** @brief Thread of each communication
 *
 *  @param arg argument passed by caller
 *  @return Void.
 */
void *chat_thread(void *arg);

#endif /* _CHAT_H */
