#include <stdio.h>
#include <sys/time.h>
#include <sys/times.h>
#include "allocate.h"

int main(int argc, char *argv[])
{
	int check = 1;
	
	struct timeval timeval;
	struct timezone timezone;
	suseconds_t start, end;
	
	struct tms cycleBuf;
	clock_t start_cycles, end_cycles;
	
	if (times(&cycleBuf) == -1)
		perror("Error: ");
		
	if (gettimeofday(&timeval, &timezone) == -1 )
		perror("Error: ");
		
	start = timeval.tv_usec;
	start_cycles = cycleBuf.tms_stime;
	
		while(check)
		check = allocate(10);
	
	
	if (times(&cycleBuf) == -1)
		perror("Error: ");
		
	if (gettimeofday(&timeval, &timezone) == -1)
		perror("Error: ");
		
	end = timeval.tv_usec;
	end_cycles = cycleBuf.tms_stime;
	
	printf("starts = %ld \nends = %ld \ntotal work = %ld \n",start, end, end - start);
	printf("sarts_c = %ld\nends_c = %ld\ntotal_c = %ld\n",start_cycles, end_cycles, end_cycles - start_cycles);
		return 0;
}
