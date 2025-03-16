/** @file ui.c
 *  @brief Chat application UI driver.
 *
 *  This file contains function definitions with 
 *  all commands used for this chat application
 *
 *  @author Phuc
 *  @bug No know bugs.
 */


#include "../include/chat.h"
#include <ifaddrs.h>  // Added for getifaddrs()

extern ClientInfo clients[MAX_CLIENTS];
extern int client_count;
extern pthread_mutex_t client_mutex;
extern int server_fd;

void ui_start(void)
{
    printf("\n====Chat Application====\n\n");
    printf("Available commands:\n");
    printf("1. help:                             Display information about the available user interface options or command manual.\n");
    printf("2. myip:                             Display the IP address of this process.\n");
    printf("3. myport:                           Display the port on which this process is listening for incoming connections.\n");
    printf("4. connect <destination> <port no> : Establishes a new TCP connection to the specified <destination> at the specified < port no>.\n");
    printf("5. list:                             Display a numbered list of all the connections this process is part of.\n");
    printf("6. terminate <connection id.>:       Terminate the connection to <connection id>.\n");
    printf("7. send <connection id.> <message>:  Send the message to the host on the connection.\n");
    printf("8. exit:                             Close all connections and terminate this process.\n");
    printf("=======================\n\n");
}

void ui_listClientConnection(void)
{
    pthread_mutex_lock(&client_mutex);
    if (client_count == 0)
    {
        printf("No clients connected\n");
    }
    else
    {
        printf("id: IP address\t\tPort No.\n");
        for (int i = 0; i < client_count; i++)
        {
            printf("%d: %s\t%d\n", i + 1, clients[i].ip_addr, clients[i].listening_port);
        }
    }
    pthread_mutex_unlock(&client_mutex);
}

void ui_run(int socket_fd, int is_server)
{
    char buffer[BUFF_SIZE];
    while (1)
    {
        int n = chat_receiveMessage(socket_fd, buffer, BUFF_SIZE);
        if (n <= 0) break;
        pthread_mutex_lock(&client_mutex);
        int listening_port = -1;
        char *peer_ip = NULL;
        for (int i = 0; i < client_count; i++)
        {
            if (clients[i].socket_fd == socket_fd)
            {
                listening_port = clients[i].listening_port;
                peer_ip = clients[i].ip_addr;
                break;
            }
        }
        pthread_mutex_unlock(&client_mutex);
        if (listening_port == -1 || peer_ip == NULL)
        {
            printf("Error: Could not find peer info for socket %d\n", socket_fd);
            break;
        }
        
        if (strcmp(buffer, "TERMINATE") == 0)
        {
            // printf("Peer requested termination\n");
            break;
        }
        else
        {
            printf("\nMessage received from %s\nSender's Port: %d\nMessage: %s\n",
                peer_ip, listening_port, buffer);
        }
    }
}

void *ui_commandHandler(void *arg)
{
    char command[BUFF_SIZE];
    while (1)
    {
        printf("Enter your command: ");
        /* Waits for user to type something (e.g., “connect 192.168.1.100 4322”).
         If user just hits Enter or something goes wrong, it skips and asks again. */
        if (fgets(command, sizeof(command), stdin) == NULL) continue;
        command[strcspn(command, "\n")] = 0;
        /* Reads command, takes the first word before space */
        char *token = strtok(command, " ");
        if (token == NULL) continue;

        if (!strcmp(token, cmd_help))
        {
            ui_start();
        }
        else if (!strcmp(token, cmd_myip))
        {
            /* ifaddrs is a list to hold all computer’s network addresses */
            struct ifaddrs *ifaddr, *ifa;
            int found = 0;
            /* Get all network interfaces and store them to ifaddr */
            if (getifaddrs(&ifaddr) == -1)
            {
                perror("getifaddrs failed");
                continue;
            }
            /* Loops through the list of network interfaces in ifaddr */
            for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
            {
                if (ifa->ifa_addr == NULL) continue;
                /* ifa->ifa_addr->sa_family == AF_INET checks if this address is an IPv4 type */
                /* strcmp(ifa->ifa_name, "lo") != 0 checks if the interface name isn’t “lo” (loopback, aka 127.0.0.1) */
                if (ifa->ifa_addr->sa_family == AF_INET && strcmp(ifa->ifa_name, "lo") != 0)
                {
                    struct sockaddr_in *addr = (struct sockaddr_in *)ifa->ifa_addr;
                    printf("%s\n", inet_ntoa(addr->sin_addr));
                    found = 1;
                    break;
                }
            }
            /* Free the list */
            freeifaddrs(ifaddr);
            if (!found)
            {
                printf("No suitable IP address found\n");
            }
        }
        else if (!strcmp(token, cmd_myport))
        {
            printf("%d\n", server_getPort());
        }
        else if (!strcmp(token, cmd_connect))
        {
            /* Read IP address and port from command, separate by space */
            char *ipAddr = strtok(NULL, " ");
            char *portStr = strtok(NULL, " ");
            if (!ipAddr || !portStr)
            {
                printf("\nUsage: connect <destination> <port no>\n");
                continue;
            }
            int portNum = atoi(portStr);
            struct sockaddr_in addr;
            inet_pton(AF_INET, ipAddr, &addr.sin_addr);
            if (server_getServerAddr() == addr.sin_addr.s_addr && server_getPort() == portNum)
            {
                printf("Error: Self-connection not allowed\n");
                continue;
            }
            /* Check if thie IP address has already connected to our server */
            pthread_mutex_lock(&client_mutex);
            int is_duplicate = 0;
            for (int i = 0; i < client_count; i++)
            {
                // if (strcmp(clients[i].ip_addr, ipAddr) == 0)
                // {
                //     is_duplicate = 1;
                //     break;
                // }
                if (clients[i].listening_port == portNum)
                {
                    is_duplicate = 1;
                    break;
                }
            }
            if (is_duplicate)
            {
                printf("Error: Duplicate connection\n");
                pthread_mutex_unlock(&client_mutex);
                continue;
            }
            pthread_mutex_unlock(&client_mutex);
            client_socketStart(ipAddr, portNum);
        }
        else if (!strcmp(token, cmd_list))
        {
            ui_listClientConnection();
        }
        else if (!strcmp(token, cmd_terminate))
        {
            char *idStr = strtok(NULL, " ");
            if (!idStr)
            {
                printf("Usage: terminate <connection id>\n");
                continue;
            }
            int id = atoi(idStr);
            pthread_mutex_lock(&client_mutex);
            if (id < 1 || id > client_count)
            {
                printf("Error: Invalid connection ID\n");
                pthread_mutex_unlock(&client_mutex);
                continue;
            }
            /* Send termination message to peer */
            if (chat_sendMessage(clients[id - 1].socket_fd, "TERMINATE") == -1)
            {
                perror("Failed to send termination message");
            }
            close(clients[id - 1].socket_fd);
            /* Shift remaining entries left */
            for (int i = id - 1; i < client_count - 1; i++)
            {
                clients[i] = clients[i + 1];
            }
            client_count--;
            printf("Connection %d terminated\n", id);
            pthread_mutex_unlock(&client_mutex);
        }
        else if (!strcmp(token, cmd_send))
        {
            char *idStr = strtok(NULL, " ");
            char *msg = strtok(NULL, "");
            if (!idStr || !msg)
            {
                printf("Usage: send <connection id> <message>\n");
                continue;
            }
            int id = atoi(idStr);
            if (strlen(msg) > MSG_SIZE)
            {
                printf("Error: Message exceeds 100 characters\n");
                continue;
            }
            pthread_mutex_lock(&client_mutex);
            if (id < 1 || id > client_count)
            {
                printf("Error: Invalid connection ID\n");
                pthread_mutex_unlock(&client_mutex);
                continue;
            }
            chat_sendMessage(clients[id - 1].socket_fd, msg);
            printf("Message sent to %d\n", id);
            pthread_mutex_unlock(&client_mutex);
        }
        else if (!strcmp(token, cmd_exit))
        {
            pthread_mutex_lock(&client_mutex);
            for (int i = 0; i < client_count; i++)
            {
                chat_sendMessage(clients[i].socket_fd, "TERMINATE");  // Notify all peers
                close(clients[i].socket_fd);
            }
            client_count = 0;
            pthread_mutex_unlock(&client_mutex);
            if (server_fd != -1)
            {
                close(server_fd);
                server_fd = -1;
            }
            printf("Chat application exiting...\n");
            exit(0);  // Exit the program cleanly
        }
        else
        {
            printf("Invalid command, try again!\n");
        }
    }
    return NULL;
}

