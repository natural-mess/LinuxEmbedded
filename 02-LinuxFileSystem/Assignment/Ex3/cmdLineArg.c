#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]) 
{
    // if user uses wrong command
    if (argc < 4) 
    {
        printf("Usage: %s <filename> <numBytes> <command> [data]\n", argv[0]);
        return 1;
    }

    // process command
    char *filename = argv[1];
    int numByte = atoi(argv[2]);
    char command = argv[3][0];
    int flag;
    char *writeData = NULL;

    // check numbyte
    if (numByte < 1) 
    {
        printf("Invalid number of bytes to read or write\n");
        return 1;
    }

    // check command
    if (command == 'w') 
    {
        if (argc < 5) 
        {
            printf("Invalid data to write\n");
            return 1;
        }
        writeData = argv[4];
        flag = O_CREAT | O_WRONLY;
    } 
    else if (command == 'r') 
    {
        flag = O_RDONLY;
    } 
    else 
    {
        printf("Invalid command\n");
        return 1;
    }

    // permission
    // Owner:   rw-  (Read, Write)
    // Group:   rw-  (Read, Write)
    // Others:  rw-  (Read, Write)
    int fd = open(filename, flag, 0666);
    if (fd < 0) 
    {
        perror("open failed");
        return 1;
    }

    if (command == 'r') 
    {
        // char *buf = malloc(numByte+1);
        char buf[numByte+1];
        if (buf == NULL) 
        {
            printf("Memory allocation failed\n");
            close(fd);
            return 1;
        }

        int readByte = read(fd, buf, numByte);
        if (readByte < 0) 
        {
            perror("Read failed");
            // free(buf);
            close(fd);
            return 1;
        }

        buf[readByte] = '\0';  // terminate string with null
        printf("Data in %s:\n%s\n", filename, buf);

        // free(buf);
    } 
    else if (command == 'w') 
    {
        int writtenByte = write(fd, writeData, numByte);
        if (writtenByte < 0) 
        {
            perror("Write failed");
            close(fd);
            return 1;
        }
        printf("Wrote %d bytes to %s\n", writtenByte, filename);
    }

    close(fd);
    return 0;
}
