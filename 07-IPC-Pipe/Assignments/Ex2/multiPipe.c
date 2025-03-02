#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MSG_SIZE1 16
#define MSG_SIZE2 12

char *msg1 = "Hello child 1, ";
char *msg2 = "I'm child 2";

int main(int argc, char const *argv[])
{
    int parent_to_child1[2];
    int child1_to_child2[2];
    pid_t pid1, pid2;
    char msg_parent2child1[MSG_SIZE1 + MSG_SIZE2];
    char msg_child12child2[MSG_SIZE1 + MSG_SIZE2];

    // Create pipes
    if (pipe(parent_to_child1) < 0 || pipe(child1_to_child2) < 0)
    {
        perror("Pipe failed");
        exit(1);
    }

    // First fork (Child 1)
    pid1 = fork();
    if (pid1 < 0)
    {
        perror("Fork child 1 failed");
        exit(1);
    }

    if (pid1 == 0) // Child 1
    { 
        // Close unused ends
        close(parent_to_child1[1]);  // write end parent_to_child1 is unused
        close(child1_to_child2[0]);  // read end child1_to_child2 is unused

        // Read from parent_to_child1[0]
        read(parent_to_child1[0], msg_parent2child1, MSG_SIZE1);

        // Print received message
        printf("Child 1: Received '%s'\n", msg_parent2child1);

        // Modify message
        strcat(msg_parent2child1, msg2);

        // Write to child1_to_child2[1]
        write(child1_to_child2[1], msg_parent2child1, MSG_SIZE1+MSG_SIZE2);

        // Close pipes, exit
        close(parent_to_child1[0]);
        close(child1_to_child2[1]);
        exit(0);
    }

    // Second fork (Child 2) - only in parent
    if (pid1 > 0)
    {
        pid2 = fork();
        if (pid2 < 0)
        {
            perror("Fork child 2 failed");
            exit(1);
        }
        if (pid2 == 0) // Child 2
        { 
            // Close unused ends
            close(child1_to_child2[1]);
            close(parent_to_child1[0]);
            close(parent_to_child1[1]);

            // Read from child1_to_child2[0]
            read(child1_to_child2[0], msg_child12child2, MSG_SIZE1+MSG_SIZE2);

            // Print message
            printf("Child 2: Received '%s'\n", msg_child12child2);

            // Exit
            close(child1_to_child2[0]);
            exit(0);
        }
    }

    // Parent (pid1 > 0 && pid2 > 0)
    if (pid1 > 0 && pid2 > 0)
    {
        // Close unused ends
        close(parent_to_child1[0]);  // read end is unused
        close(child1_to_child2[0]);
        close(child1_to_child2[1]);

        // Write to parent_to_child1[1]
        printf("Parent is sending to child 1\n");
        write(parent_to_child1[1], msg1, MSG_SIZE1);

        // Close pipe
        close(parent_to_child1[1]);  // close write end

        // Wait for both children
        wait(NULL);
        wait(NULL);
    }

    return 0;
}