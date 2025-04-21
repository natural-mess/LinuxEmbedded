#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <stdbool.h>

#define COUNT_SIG 10

void sigalrm_handler()
{
    static int countAlarm = 0;
    countAlarm++;
    printf("Timer: %d seconds\n", countAlarm);
    if (countAlarm >= COUNT_SIG)
        exit(EXIT_SUCCESS);

    alarm(1);
}

int main(void)
{
    if (signal(SIGALRM, sigalrm_handler) == SIG_ERR) 
    {
		fprintf(stderr, "Cannot handle SIGALRM\n");
		exit(EXIT_FAILURE);
	}
    alarm(1);
    while(1) 
    {
        pause();
    }
    return 0;
}