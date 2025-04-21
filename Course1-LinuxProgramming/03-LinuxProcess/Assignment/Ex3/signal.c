#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void SIGUSR1_handler(int signum)
{
    printf("Child process has received the signal from parent process\n");
}

int main(int argc, char const *argv[])
{
    pid_t child_pid;
    int counter = 1;

    printf("Init value of counter: %d\n", counter);

    child_pid = fork();
    if (child_pid >= 0)
    {
        if (0 == child_pid) // child process
        {
            printf("\nIm the child process, counter: %d\n", ++counter);
            printf("My PID is: %d, my parent PID is: %d\n", getpid(), getppid());
            signal(SIGUSR1, SIGUSR1_handler);
            while (1);
        }
        else // parent process
        {
            printf("\nIm the parent process, counter: %d\n", ++counter);
            printf("My PID is: %d, my child PID is: %d\n", getpid(), child_pid);
            printf("Sending SIGUSR1 signal to child process in 3 seconds...\n");
            fflush(stdout);  // Ensure message is printed before sleeping
            sleep(3);
            kill(child_pid, SIGUSR1);
            wait(NULL); // prevent child process become zombie process
        }
    } 
    else
    {
        printf("fork() failed\n");      // fork() return -1 if fail
    }

    return 0;
}
