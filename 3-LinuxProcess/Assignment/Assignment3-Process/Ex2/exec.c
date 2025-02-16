#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) 
{
    if (argc < 2)
    {
        printf("Usage: %s <1|2>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "1") == 0)
    {
        execlp("ls", "ls", NULL);
    }
    else if (strcmp(argv[1], "2") == 0)
    {
        char *buf[] = {"date", NULL};
        execvp("date", buf);
    }
    else
    {
        printf("Invalid argument\n");
    }

    return 0;   
}