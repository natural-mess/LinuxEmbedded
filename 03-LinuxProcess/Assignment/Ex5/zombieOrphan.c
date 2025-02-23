#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    pid_t child_zombie;
    int counter = 1;

    // printf("Init value of counter: %d\n", counter);
    
    child_zombie = fork();
    if (child_zombie >= 0)
    {
        if (0 == child_zombie) // child process
        {
            // printf("\nIm the child process, counter: %d\n", ++counter);
            printf("I'm child process, my PID is: %d\n", getpid());
            printf("I'm a zombie process, my PID is: %d\n", getpid());
        }
        else // parent process
        {
            // printf("\nIm the parent process, counter: %d\n", ++counter);
            printf("I'm parent process. My PID is: %d, my child PID is: %d\n", getpid(), child_zombie);
            printf("I will enter infinite loop to create zombie process\n");
            while(1);
        }
    } 
    else
    {
        printf("fork() failed\n");      // fork() return -1 if fail
    }
    printf("\n");
    pid_t child_orphan = fork();
    if (child_orphan >= 0)
    {
        if (0 == child_orphan) // child process
        {
            // printf("\nIm the child process, counter: %d\n", ++counter);
            printf("I'm child process, my PID is: %d\n", getpid());
            sleep(2);
            printf("I'm an orphan process, my PID is: %d\n", getpid());
        }
        else // parent process
        {
            // printf("\nIm the parent process, counter: %d\n", ++counter);
            printf("I'm parent process. My PID is: %d, my child PID is: %d\n", getpid(), child_orphan);
            printf("I will exit immediately to create an orphan process\n");
            exit(0);
        }
    } 
    else
    {
        printf("fork() failed\n");      // fork() return -1 if fail
    }

    return 0;
}
