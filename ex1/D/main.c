#define _BSD_SOURCE
#include <stdio.h>
#include <sys/time.h>
//#include <busy_wait_delay.h>

int main(int argc, char *argv[])
{
	/*
		printf("hmm!\n");
		usleep(5000000);
		printf("more hmm!!\n");
	*/
		printf("hmm!\n");
		busy_wait_delay(5);
		printf("more hmm!!\n");
		
		return 0;
}
