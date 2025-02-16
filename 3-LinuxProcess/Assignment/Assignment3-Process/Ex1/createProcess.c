#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
        }
        else // parent process
        {
            printf("\nIm the parent process, counter: %d\n", ++counter);
            printf("My PID is: %d, my child PID is: %d\n", getpid(), child_pid);
            while (1);
        }
    } 
    else
    {
        printf("fork() failed\n");      // fork() return -1 if fail
    }

    return 0;
}
