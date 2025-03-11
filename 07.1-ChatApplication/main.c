#include "include/chat.h"

int main(int argc, char *argv[])
{
    int portNum;

    /* Read port number from command line */
    if (argc < 2)
    {
        printf("No port provided\ncommand: ./server <port number>\n");
        exit(EXIT_FAILURE);
    }
    system("clear");
    portNum = atoi(argv[1]);

    /* Print instruction */
    ui_start();

    /* Command handler */
    ui_commandHandler();

    return 0;
}
