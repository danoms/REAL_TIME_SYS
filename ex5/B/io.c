#define _GNU_SOURCE

#include "io.h"
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

#include <sched.h>
#include <unistd.h>


#define NUM_WORK_THREADS 3
#define DISTURBANCE_THREADS	10
#define TEST_SIZE 	1100
#define CPU_NUMBER 	0

static comedi_t *it_g = NULL;
int test_runs = 1;

int io_init()
{
	int i;
	int res;

	it_g = comedi_open("/dev/comedi0");

	if (it_g == NULL) return -1;

	// init 3 inputs and 3 outputs
	for(i=1; i<4; i++)
	{
		res = comedi_dio_config(it_g, IO_DEV, DI(i), COMEDI_INPUT);
		if (res<0) return res;
		res = comedi_dio_config(it_g, IO_DEV, DO(i), COMEDI_OUTPUT);
		if (res<0) return res;
		res = comedi_dio_write(it_g, IO_DEV, DO(i), 0);
		if (res<0) return res;
	}

	// set outputs to high
	for(i=1; i<4; i++)
	{
		comedi_dio_write(it_g, IO_DEV, DO(i), 1);
	}

	return 1;
}

void io_write(int channel, int value)
{
	if(channel >= 1 && channel <= 3)
	{
		if(value == 0) comedi_dio_write(it_g, IO_DEV, DO(channel), 0);
		else comedi_dio_write(it_g, IO_DEV, DO(channel), 1);
	}
	else
	{
		printf("Incorrect io channel\n");
	}
}

int io_read(int channel)
{
	unsigned int data=0;

	if(channel >= 1 && channel <= 3)
	{
		comedi_dio_read(it_g, IO_DEV, DI(channel), &data);
		return (int)data;
	}
	else
	{
		printf("Incorrect io channel : %d \n", channel);
		return -1;
	}


}

int set_cpu(int cpu_number)
{
	// setting cpu set to the selected cpu
	cpu_set_t cpu;
	CPU_ZERO(&cpu);
	CPU_SET(cpu_number, &cpu);

	// set cpu set to current thread and return
	return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

void *test_pin(void *index_p)
{
	set_cpu(CPU_NUMBER);
	int counter = 0;

	while(test_runs) {
	   if (!io_read(*(int*)index_p))
		{
			io_write(*(int*)index_p,0);
			counter++;
			usleep(5000);
			io_write(*(int*)index_p,1);
			if (counter > TEST_SIZE)
			{
				test_runs = 0;	
			}
		}
	}
	
	return 0;
}



void *count_forever(void *value_p)
{
	set_cpu(CPU_NUMBER);
	
	int garbage;
	while(test_runs) {
	    garbage = garbage + 4;
	    if (garbage > 500000)
	    {
	    	garbage = 0;
	    }
	}
	return 0;
}

int main(int argc, char const *argv[])
{

	/*initialize pins on board*/
	io_init();

	/*create thread array*/
	pthread_t work_threads[NUM_WORK_THREADS];
	pthread_t dist_threads[DISTURBANCE_THREADS];
	int thread_args[NUM_WORK_THREADS];
	int result_code;

	/*create work_threads one by one*/
	for (int index_work = 1; index_work <= NUM_WORK_THREADS; index_work++)
	{
		printf("In main: creating thread %d\n", index_work );
		thread_args[index_work-1] = index_work;
		result_code = pthread_create( &work_threads[index_work], NULL, test_pin, &thread_args[index_work-1]);
		assert(!result_code);
	}

	// for (int index = 0; index < DISTURBANCE_THREADS; index++)
	// {		
	// 	printf("In main: creating disturbance thread %d\n", index );
	// 	result_code = pthread_create( &dist_threads[index], NULL, count_forever, NULL);
	// 	assert(!result_code);
	// }

	/*wait for each work thread to complete*/
	for (int index_work = 1; index_work <= NUM_WORK_THREADS; index_work++)
	{
		result_code = pthread_join( work_threads[index_work], NULL);
		assert(!result_code);
		printf("In main: Work thread %d has completed\n", index_work );
	}

	/*wait for disturbance threads to complete*/
	// for (int index = 0; index < DISTURBANCE_THREADS; index++)
	// {
	// 	result_code = pthread_join( dist_threads[index], NULL);
	// 	assert(!result_code);
	// 	printf("In main: Disturbance thread %d has completed!\n", index);
	// }

	printf("In main: All work_threads completed successfully!\n");
	exit(EXIT_SUCCESS);
}