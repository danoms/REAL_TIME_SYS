//#define _GNU_SOURCE

#include "io.h"
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <sched.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

#include <xenomai/native/task.h>
#include <xenomai/native/timer.h>
#include <sys/mman.h>
#include <rtdk.h>

#define NUM_WORK_THREADS 3
#define DISTURBANCE_THREADS	10
#define TEST_SIZE 	1010
#define CPU_NUMBER 	0
#define PERIOD_US 10000
#define DISTURBANCE 1

#define TIMESLEEP 1000000000

static comedi_t *it_g = NULL;
int test_runs = 1;


RT_TASK hello_task;
RT_TASK hello_task2;
RT_TASK hello_task3;


// function to be executed by task
void helloWorld(void *arg)
{
  RT_TASK_INFO curtaskinfo;
  rt_task_set_periodic(NULL, TM_NOW, TIMESLEEP);

  printf("Hello World!\n");

  // inquire current task
  rt_task_inquire(NULL,&curtaskinfo);
    
    while(1) {
        rt_task_wait_period(NULL);
        printf("Task name : %s \n", curtaskinfo.name);
    }
  // print task name
  
}

// int main(int argc, char* argv[])
// {
//   mlockall(MCL_CURRENT|MCL_FUTURE);
//   char  str[10];

//   printf("start task\n");
//   sprintf(str,"hello");

//    Create task
//    * Arguments: &task,
//    *            name,
//    *            stack size (0=default),
//    *            priority,
//    *            mode (FPU, start suspended, ...)
   
//   rt_task_create(&hello_task, str, 0, 50, 0);

//   /*  Start task
//    * Arguments: &task,
//    *            task function,
//    *            function argument
//    */
//   rt_task_start(&hello_task, &helloWorld, 0);
// }


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

int set_cpu(int cpu_number)
{
	// setting cpu set to the selected cpu
	cpu_set_t cpu;
	CPU_ZERO(&cpu);
	CPU_SET(cpu_number, &cpu);

	// set cpu set to current thread and return
	return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

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

void test_pin(void *index_p)
{
	//RT_TASK_INFO curtaskinfo;
    rt_task_set_periodic(NULL, TM_NOW, PERIOD_US);

    //rt_task_inquire(NULL,&curtaskinfo);

	// set_cpu(CPU_NUMBER);
	int counter = 0;

	//struct timespec period_time;
	//clock_gettime(CLOCK_REALTIME, &period_time);
	//timespec_add_us(&period_time, PERIOD_US);

	while(test_runs) {
		//clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &period_time, NULL);
		//timespec_add_us(&period_time, PERIOD_US);
		rt_task_wait_period(NULL);
		//printf("Task name 1: %s, %d\n", curtaskinfo.name, *(int*)index_p);

	   if (!io_read(*(int*)index_p))
		{
			io_write(*(int*)index_p,0);
			counter++;
			usleep(1);
			io_write(*(int*)index_p,1);
			if (counter > TEST_SIZE)
			{
				test_runs = 0;	
			}
		}
	}
	
	//return 0;
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
	mlockall(MCL_CURRENT|MCL_FUTURE);
	/*initialize pins on board*/
	io_init();
	char  str[10];

    printf("start task\n");
    sprintf(str,"hello");

	/*create thread array*/
	//pthread_t work_threads[NUM_WORK_THREADS];
	pthread_t dist_threads[DISTURBANCE_THREADS];
	//int thread_args[NUM_WORK_THREADS];
	int result_code;

	/*create work_threads one by one*/
	// for (int index = 1; index <= NUM_WORK_THREADS; index++)
	// {
	// 	printf("In main: creating thread %d\n", index );
	// 	thread_args[index-1] = index;
	// 	result_code = pthread_create( &work_threads[index], NULL, test_pin, &thread_args[index-1]);
	// 	assert(!result_code);
	// }
    int index;
	if (DISTURBANCE)
	{
		for (index = 0; index < DISTURBANCE_THREADS; index++)
		{		
			printf("In main: creating disturbance thread %d\n", index );
			result_code = pthread_create( &dist_threads[index], NULL, count_forever, NULL);
			assert(!result_code);
		}
	}

	int arr[3] = {1,2,3};
	if (rt_task_create(&hello_task, str, 0, 50, T_CPU(0)))
	 {
	 	printf("something wrong\n");
	 } 
	

	if (rt_task_create(&hello_task2, "name 2", 0, 50, T_CPU(0)))
	 {
	 	printf("something wrong 2\n");
	 } 
	

	if (rt_task_create(&hello_task3, "name 3", 0, 50, T_CPU(0)))
	 {
	 	printf("something wrong 3\n");
	 } 
	rt_task_start(&hello_task2, &test_pin, arr + 1);
	rt_task_start(&hello_task, &test_pin, arr);
	rt_task_start(&hello_task3, &test_pin, arr + 2);
	

	/*wait for each work thread to complete*/
	// for (int index = 1; index <= NUM_WORK_THREADS; index++)
	// {
	// 	result_code = pthread_join( work_threads[index], NULL);
	// 	assert(!result_code);
	// 	printf("In main: Work thread %d has completed\n", index );
	// }

	if (DISTURBANCE)
	{
		/*wait for disturbance threads to complete*/
		for (index = 0; index < DISTURBANCE_THREADS; index++)
		{
			result_code = pthread_join( dist_threads[index], NULL);
			assert(!result_code);
			printf("In main: Disturbance thread %d has completed!\n", index);
		}
	}
	
	while(1)
	{

	}
	printf("In main: All work_threads completed successfully!\n");
	exit(EXIT_SUCCESS);
}
