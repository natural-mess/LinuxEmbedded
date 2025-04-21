#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define MSG_SIZE    100

char *msg1 = "Hello from parent process";

int main(int argc, char const *argv[])
{
    /* code */
    char in_buff[MSG_SIZE];
    int num_read = 0;
    int fds[2];
    pid_t child_pid;

    if (pipe(fds) < 0)
    {
        printf("pipe() unsuccessfully\n");
        exit(1);
    }

    child_pid = fork();
    if (child_pid >= 0)
    {
        if (child_pid == 0)
        {
            printf("Child: I am Child\n"); // reader

            // close write end
            close(fds[1]);
            
            // Read from fds[0]
            read(fds[0], in_buff, MSG_SIZE);

            // Print received message
            printf("Child: Received '%s'\n", in_buff);
            printf("Child: Number of character: %ld\n", strlen(in_buff));

            // close read end
            close(fds[0]);
            exit(0);
        }
        else
        {
            printf("Parent: I am Parent\n"); // writer

            // close read end
            close(fds[0]);

            // Write into pipe
            write(fds[1], msg1, MSG_SIZE);

            // close write end
            close(fds[1]);

            // exit
            wait(NULL);
        }
    }
    else
    {
        perror("fork() unsuccessfully\n");
        exit(1);
    }

    return 0;
}