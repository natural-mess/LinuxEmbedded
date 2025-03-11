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

void ui_start(void)
{
    printf("====Chat Application====\n\n");
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

void ui_commandHandler(void)
{
    char command[BUFF_SIZE];
    while(1)
    {
        printf("Enter your command: ");
        if (fgets(command, sizeof(command), stdin) == NULL)
            continue;  // Ignore empty input

        command[strcspn(command, "\n")] = 0;  // Remove newline
        
        // Get first word (command)
        char *token = strtok(command, " ");
        if (token == NULL)
            continue;  // Ignore empty input

        if (!strcmp(token, cmd_help))
        {
            ui_start();
        }
        else if (!strcmp(token, cmd_myip))
        {

        }
        else if (!strcmp(token, cmd_myport))
        {

        }
        else if (!strcmp(token, cmd_connect))
        {

        }
        else if (!strcmp(token, cmd_list))
        {

        }
        else if (!strcmp(token, cmd_terminate))
        {

        }
        else if (!strcmp(token, cmd_send))
        {

        }
        else if (!strcmp(token, cmd_exit))
        {
            break;
        }
        else
        {
            printf("Invalid command, try again!\n");
        }
    }
    
}

