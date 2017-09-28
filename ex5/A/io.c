#include "io.h"
#include <strio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

#define NUM_THREADS 3
#define TEST_SIZE 	1000

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
		printf("Incorrect io channel\n");
		return -1;
	}
}

void *test_pin(void *index_p)
{
	int counter = 0;

	while(test_runs) {
	   if (!io_read(*index_p))
		{
			io_write(*index_p,0);
			counter++;
			usleep(5000);
			io_write(*index_p,1);
			if (counter >= TEST_SIZE)
			{
				test_runs = 0;	
			}
		}
	}
	
	return 0;
}


int main(int argc, char const *argv[])
{
	/*initialize pins on board*/
	io_init();

	/*create thread array*/
	pthread_t threads[NUM_THREADS];
	int thread_args[NUM_THREADS];
	int result_code;
	unsigned index;

	/*create threads one by one*/
	for (int index = 1; index <= NUM_THREADS; index++)
	{
		printf("In main: creating thread %d\n", index );
		thread_args[index] = index;
		result_code = pthread_create( &threads[index], NULL, test_pin, &thread_args[index]);
		assert(!result_code);
	}

	/*wait for each thread to complete*/
	for (int index = 1; index <= NUM_THREADS; index++)
	{
		result_code = pthread_join( &threads[index], NULL);
		assert(!result_code);
		printf("In main: thread %d has completed\n", index );
	}

	printf("In main: All threads completed successfully!\n");
	exit(EXIT_SUCCESS);
}