#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <signal.h>

#define COUNT_SIG 5

void sigusr1_handler()
{
    printf("Received signal from parent\n");
    kill(getppid(), SIGUSR2);
}

// Signal handler for the parent process
void sigusr2_handler()
{
    // Parent received acknowledgment from child
}

int main()
{
    pid_t childPid = fork();
    if (childPid >= 0)
    {
        if (childPid == 0)
        {
            // child process
            sleep(1);
            printf("I'm child process, my pid: %d\n", getpid());
            fflush(stdout); // Force the output to appear immediately
            if (signal(SIGUSR1, sigusr1_handler) == SIG_ERR)
            {
                fprintf(stderr, "Cannot handle SIGUSR1\n");
                exit(EXIT_FAILURE);
            }

            if (kill(getppid(), SIGUSR2) < 0)
            {
                perror("Send signal to parent process failed\n");
            }

            while (1)
            {
                pause();  // Wait for signals
            }
        }
        else
        {
            // parent process
            printf("I'm parent process, my pid: %d, my child pid: %d\n", getpid(), childPid);
            if (signal(SIGUSR2, sigusr2_handler) == SIG_ERR)
            {
                fprintf(stderr, "Cannot handle SIGUSR2\n");
                exit(EXIT_FAILURE);
            }
            pause();

            for (int i = 0; i < COUNT_SIG; i++)
            {
                int killStatus = kill(childPid, SIGUSR1);
                
                if (killStatus < 0)
                {
                    perror("Send signal to child process failed\n");
                }
                pause();
            }
            printf("Killing child process...\n");
            if (kill(childPid, SIGKILL) < 0)
            {
                perror("Send signal to child process failed\n");
            }
            wait(NULL);
            printf("Exiting parent process...\n");
            exit(EXIT_SUCCESS);
        }
    }
    else
    {
        printf("fork() failed\n");
    }
    return 0;
}