#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <stdbool.h>

void sigtstp_handler(int signum)
{
    printf("\nSIGTSTP ignored\n");
}

int main(void)
{
    if (signal(SIGTSTP, sigtstp_handler) == SIG_ERR)
    {
        fprintf(stderr, "Cannot handle SIGTSTP\n");
        exit(EXIT_FAILURE);
    }
    printf("Process is running, Ctrl+Z has no effect\n");
    while(1)
    {
        pause();
    }
    return 0;
}