#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <poll.h>

volatile sig_atomic_t sigint_flag = 0;
volatile sig_atomic_t sigterm_flag = 0;

// Signal handler
void signal_handler(int signum) 
{
    if (signum == SIGINT)
    {
        sigint_flag = 1;
    } else if (signum == SIGTERM)
    {
        sigterm_flag = 1;
    }
}

int main(void)
{
    if (signal(SIGINT, signal_handler) == SIG_ERR)
    {
        fprintf(stderr, "Cannot handle SIGINT\n");
        exit(EXIT_FAILURE);
    }

    if (signal(SIGTERM, signal_handler) == SIG_ERR)
    {
        fprintf(stderr, "Cannot handle SIGTERM\n");
        exit(EXIT_FAILURE);
    }
    
    struct pollfd fds[1];
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;

    while (1) 
    {
        int ready = poll(fds, 1, 3000);  // wait for 3 seconds

        if (ready > 0)
        {
            if (fds[0].revents & POLLIN)
            {
                char input[100];
                if (fgets(input, sizeof(input), stdin) != NULL)
                {
                    printf("User input: %s", input);
                }
            }
        }

        // Check signals
        if (sigint_flag)
        {
            printf("\nSIGINT received.\n");
            sigint_flag = 0;
        }
        if (sigterm_flag)
        {
            exit(EXIT_SUCCESS);
        }
    }
    return 0;
}