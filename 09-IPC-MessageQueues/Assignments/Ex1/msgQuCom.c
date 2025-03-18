#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>    // For O_* flags
#include <mqueue.h>   // POSIX message queues
#include <string.h>
#include <sys/wait.h>

int main()
{
    pid_t child_pid;

    // Queue attributes
    struct mq_attr attr;
    attr.mq_flags = 0;        // Blocking (default)
    attr.mq_maxmsg = 10;      // Max 10 messages
    attr.mq_msgsize = 100;    // Each message up to 100 bytes
    attr.mq_curmsgs = 0;      // Current count (kernel sets this)

    // Open or create the queue
    mqd_t mq = mq_open("/myqueue", O_CREAT | O_WRONLY, 0644, &attr);
    if (mq == -1)
    {
        perror("mq_open failed");
        return 1;
    }

    child_pid = fork();
    if (child_pid >= 0)
    {
        if (0 == child_pid) // child process
        {
            // printf("\nIm the child process, counter: %d\n", ++counter);
            // printf("My PID is: %d, my parent PID is: %d\n", getpid(), getppid());
            // Open the queue for reading
            mqd_t mq = mq_open("/myqueue", O_RDONLY);
            if (mq == -1)
            {
                perror("mq_open failed");
                return 1;
            }

            // Receive a message
            char buffer[100];
            struct mq_attr child_attr;
            unsigned int priority;
            mq_getattr(mq, &child_attr);
            ssize_t bytes = mq_receive(mq, buffer, child_attr.mq_msgsize, &priority);
            if (bytes == -1)
            {
                perror("mq_receive failed");
                return 1;
            }
            printf("Received (priority %u): %s\n", priority, buffer);

            // Close the queue in child
            mq_close(mq);

            exit(0);
        }
        else // parent process
        {
            // printf("\nIm the parent process, counter: %d\n", ++counter);
            // printf("My PID is: %d, my child PID is: %d\n", getpid(), child_pid);

            // Send a message
            char *msg = "Hello child, I'm your parent";
            unsigned int priority = 1;  // Higher = more urgent
            if (mq_send(mq, msg, strlen(msg) + 1, priority) == -1)
            {
                perror("mq_send failed");
                return 1;
            }

            // printf("Message sent from parent: %s\n", message.mtext);
            wait(NULL);

            // Close and unlink the queue
            mq_close(mq);
            mq_unlink("/myqueue");
        }
    } 
    else
    {
        printf("fork() failed\n");      // fork() return -1 if fail
    }
    
    return 0;
}
