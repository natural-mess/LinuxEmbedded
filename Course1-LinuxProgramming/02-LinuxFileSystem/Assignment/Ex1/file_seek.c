#include <stdio.h> 
#include <string.h> 
#include <unistd.h> 
#include <fcntl.h> 

int main()
{
    int fd;
    const char filename[] = "text.txt";
    char oldBuf[] = "Existing content: Hello World\n";
    char newBuf[] = "New content: Goodbye\n";

    // open file
    // text.txt exists already
    // permission
    // Owner:   rw-  (Read, Write)
    // Group:   rw-  (Read, Write)
    // Others:  rw-  (Read, Write)
    fd = open(filename, O_CREAT | O_WRONLY | O_APPEND, 0666);
    if (fd < 0)
    {
        printf("Failed to open %s\n", filename);
        perror("open failed");
        return 0;
    }

    int writeRes1 = write(fd, oldBuf, strlen(oldBuf));
    if (writeRes1 < 0)
    {
        printf("Failed to write %s to %s\n", oldBuf, filename);
        // perror("write 1 failed");
        close(fd);
        return 1;
    }
    printf("Wrote %d bytes to %s\n", writeRes1, filename);

    int seekRes = lseek(fd, 0, SEEK_SET);
    if (seekRes < 0)
    {
        printf("Failed to set cursor in %s\n", filename);
        close(fd);
        return 1;
    }
    printf("lseek() returned %d, but it will be ignored when writing.\n", seekRes);

    int writeRes2 = write(fd, newBuf, strlen(newBuf));
    if (writeRes2 < 0)
    {
        printf("Failed to write %s to %s\n", newBuf, filename);
        // perror("write 2 failed");
        close(fd);
        return 1;
    }

    printf("Wrote %d bytes to %s\n", writeRes2, filename);

    close(fd);

    return 0;
}