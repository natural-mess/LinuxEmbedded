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

    // Open or create the queue to send message
    mqd_t mq_char = mq_open("/char", O_CREAT | O_RDWR, 0666, &attr);
    if (mq_char == -1)
    {
        perror("mq_open failed");
        return 1;
    }

    // Open or create the queue to send length
    mqd_t mq_count = mq_open("/count", O_CREAT | O_RDWR, 0666, &attr);
    if (mq_count == -1)
    {
        perror("mq_open failed");
        return 1;
    }

    child_pid = fork();
    if (child_pid >= 0)
    {
        if (0 == child_pid) // child process
        {
            // Receive a message
            char buffer[100];
            struct mq_attr child_attr;
            unsigned int priority;
            mq_getattr(mq_char, &child_attr);
            ssize_t bytes = mq_receive(mq_char, buffer, child_attr.mq_msgsize, &priority);
            if (bytes == -1)
            {
                perror("mq_receive failed");
                return 1;
            }
            // printf("Received: %s\n", buffer);

            // child open queue again for read only
            mqd_t mq_count = mq_open("/count", O_WRONLY);
            if (mq_count == -1)
            {
                perror("mq_open /count failed");
                return 1;
            }

            // Send a message
            int len = strlen(buffer);
            char msg[100];
            // convert int to string
            snprintf(msg, sizeof(msg), "%d", len);
            if (mq_send(mq_count, msg, strlen(msg) + 1, priority) == -1)
            {
                perror("mq_send failed");
                return 1;
            }

            // Close the queue in child
            mq_close(mq_char);
            mq_close(mq_count);

            exit(0);
        }
        else // parent process
        {
            // Send a message
            char *msg = "Hello child, I'm your parent";
            unsigned int priority = 1;  // Higher = more urgent
            if (mq_send(mq_char, msg, strlen(msg) + 1, priority) == -1)
            {
                perror("mq_send failed");
                return 1;
            }

            // parent open queue again for read only
            mqd_t mq_count = mq_open("/count", O_RDONLY);
            if (mq_count == -1)
            {
                perror("mq_open /count failed");
                return 1;
            }

            // Receive a message
            char buffer[100];
            struct mq_attr parent_attr;
            mq_getattr(mq_count, &parent_attr);
            ssize_t bytes = mq_receive(mq_count, buffer, parent_attr.mq_msgsize, &priority);
            if (bytes == -1)
            {
                perror("mq_receive failed");
                return 1;
            }
            printf("Received: %s\n", buffer);

            // printf("Message sent from parent: %s\n", message.mtext);
            wait(NULL);

            // Close and unlink the queue
            mq_close(mq_char);
            mq_close(mq_count);
            mq_unlink("/char");
            mq_unlink("/count");
        }
    } 
    else
    {
        printf("fork() failed\n");      // fork() return -1 if fail
    }
    
    return 0;
}
