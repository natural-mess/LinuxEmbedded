#include "include/chat.h"

int server_fd = -1;  // Global for myport, terminate

int main(int argc, char *argv[])
{
    /* Read port number from command line */
    if (argc < 2)
    {
        printf("No port provided\ncommand: ./chat <port number>\n");
        exit(EXIT_FAILURE);
    }
    // system("clear");
    int portNum = atoi(argv[1]);

    /* Start socket as a server */
    server_socketStart(portNum);

    /* Print instruction */
    ui_start();

    /* Command handler */
    pthread_t ui_thread;
    if (pthread_create(&ui_thread, NULL, (void *(*)(void *))ui_commandHandler, NULL) != 0)
    {
        perror("UI thread creation failed");
        exit(EXIT_FAILURE);
    }
    pthread_detach(ui_thread);

    // Main thread could handle other tasks or just wait
    while (1)
    {
        sleep(1);  // Prevent main from exiting immediately
    }

    return 0;
}
