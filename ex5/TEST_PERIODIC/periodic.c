#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <time.h>

void timespec_add_us(struct timespec *t, long us)
{
	// add microseconds to timespecs nanosecond counter
	t->tv_nsec += us*1000;

	// if wrapping nanosecond counter, increment second counter
	if (t->tv_nsec > 1000000000)
	{
		t->tv_nsec = t->tv_nsec - 1000000000;
		t->tv_sec += 1;
	}
}

void *print_str(void *str)
{
	char *some_string = (char*)str;
	char string[] = "halllo!";
	struct timespec time;
	clock_gettime(CLOCK_REALTIME, &time);

	int check_sleep;
	while(1) {
		timespec_add_us(&time, 1000000);
	    check_sleep = clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &time, NULL);
	    if (!check_sleep)
	    {
	    	printf("sleeping was ok! \n");
	    }
	    printf("Print string ==> %s, aand it's done!!\n", (char*)str);
	}
}

int main(int argc, char const *argv[])
{
	pthread_t thread;
	int result_code;
	char *my_string = "Julius Ceasaarr!!";
	
	/* creating thread */
	result_code = pthread_create( &thread, NULL, print_str, my_string);
	assert(!result_code);
	printf("In main: Thread created!\n");

	/* joining threads */
	result_code = pthread_join( thread, NULL);
	assert(!result_code);
	printf("In main: All done!\n");
	return 0;
}