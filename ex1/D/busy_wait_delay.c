#include <sys/times.h>
#include <unistd.h>

void busy_wait_delay(int seconds)
{
	int i, dummy;
	int tps = sysconf(_SC_CLK_TCK);
	clock_t start;
	struct tms exec_time;
	
	times(&exec_time);
	start = exec_time.tms_utime;
	
	while( (exec_time.tms_utime - start) < (seconds * tps) )
	{
		for (i=0; i<1000; i++)
		{
			dummy = i;
		}
		times(&exec_time);
	}
}
