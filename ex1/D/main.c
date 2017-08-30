#define _BSD_SOURCE
#include <stdio.h>
#include <sys/time.h>
//#include <busy_wait_delay.h>

int main(int argc, char *argv[])
{
	/*
		printf("wtf!\n");
		usleep(5000000);
		printf("more wtf!!");
	*/
		printf("wtf!\n");
		busy_wait_delay(5);
		printf("more wtf!!");
		
		return 0;
}
