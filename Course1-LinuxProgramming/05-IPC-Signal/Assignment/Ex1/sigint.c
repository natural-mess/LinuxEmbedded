#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#define COUNT_SIG 3

void sigint_handler()
{
    static int countSigInt = 0;
    countSigInt++;
    printf("SIGINT received\n");
    if (countSigInt >= COUNT_SIG)
    {    
        exit(EXIT_SUCCESS);
    }
}

int main(void)
{
    if (signal(SIGINT, sigint_handler) == SIG_ERR) 
    {
		fprintf(stderr, "Cannot handle SIGINT\n");
		exit(EXIT_FAILURE);
	}
    while(1) 
    {
        pause();  // Sleep until a signal is received.
    }
    return 0;
}