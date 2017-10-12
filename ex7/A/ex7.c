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
#include <xenomai/native/sem.h>
#include <xenomai/native/mutex.h>


#define MODE 	S_PRIO // S_PRIO

#define H 		40
#define L 		10
#define M 		25 		

RT_SEM semaphore;
RT_SEM start;

RT_MUTEX mutex;


unsigned long sem_count = 1;

RT_TASK low_task;
RT_TASK medium_task;
RT_TASK high_task;



int rt_task_sleep_ms(unsigned long delay);
void print_pri(RT_TASK *task, char *s);
void busy_wait_ms(unsigned long delay);


void task_func(void *arg)
{
  RT_TASK_INFO curtaskinfo;
  // rt_task_set_periodic(NULL, TM_NOW, TIMESLEEP);

  // printf("Hello World!\n");

  // inquire current task
  rt_task_inquire(NULL,&curtaskinfo);
	rt_printf("Task %s takes semaphore.\n", curtaskinfo.name);

    rt_sem_p(&semaphore, TM_INFINITE);

	rt_printf("Task %s was released.\n", curtaskinfo.name);

    
  // print task name
  
}

void task_func2(void *arg)
{
  RT_TASK_INFO curtaskinfo;
  // rt_task_set_periodic(NULL, TM_NOW, TIMESLEEP);

  // printf("Hello World!\n");

  // inquire current task
  rt_task_inquire(NULL,&curtaskinfo);

    rt_task_sleep_ms(10000);
	rt_printf("Task %s broadcasts sem.\n", curtaskinfo.name);

    rt_sem_broadcast(&semaphore);/* broadcast semaphore */

	rt_printf("Task %s done broadcasting sem.\n", curtaskinfo.name);
	rt_task_sleep_ms(1000);
	rt_sem_delete(&semaphore);
    
  // print task name
  
}

void low_task_func(void *arg) {
	// RT_TASK_INFO curtaskinfo;
	// rt_task_inquire(NULL,&curtaskinfo);

	// rt_printf("Task %s takes semaphore.\n", curtaskinfo.name);
	rt_sem_p(&start, TM_INFINITE);

	print_pri(&low_task, "low : semaphore taken\n");
	rt_sem_p(&semaphore, TM_INFINITE);
	// rt_mutex_acquire(&mutex, TM_INFINITE);

	print_pri(&high_task, "low : busy waiting\n");
	busy_wait_ms(3);
	print_pri(&low_task, "low : releasing semaphore\n");
	rt_sem_v(&semaphore);
	// rt_mutex_release(&mutex);
	print_pri(&low_task, "low : done\n");

	// rt_printf("Task %s releases semaphore.\n", curtaskinfo.name);

}

void medium_task_func(void *arg) {
	rt_sem_p(&start, TM_INFINITE);

	print_pri(&medium_task, "med : sleeping\n");
	rt_task_sleep_ms(1);
	print_pri(&medium_task, "med : busy waiting\n");
	busy_wait_ms(5);
	print_pri(&medium_task, "med : done\n");
}

void high_task_func(void *arg) {
	rt_sem_p(&start, TM_INFINITE);

	print_pri(&high_task, "high : sleeping\n");
	rt_task_sleep_ms(2);
	print_pri(&high_task, "high : taking semaphore\n");
	rt_sem_p(&semaphore, TM_INFINITE);
	// rt_mutex_acquire(&mutex, TM_INFINITE);

	print_pri(&high_task, "high : got semaphore\n");
	print_pri(&high_task, "high : busy waiting\n");
	busy_wait_ms(2);
	print_pri(&high_task, "high : releasing semaphore\n");
	rt_sem_v(&semaphore);
	// rt_mutex_release(&mutex);
	print_pri(&high_task, "high : done\n");
}

void print_pri(RT_TASK *task, char *s)
{
	struct rt_task_info temp;
	rt_task_inquire(task, &temp);
	rt_printf("b:%i c:%i ", temp.
	bprio, temp.cprio);
	rt_printf(s);
}

void busy_wait_ms(unsigned long delay){
	unsigned long count = 0;

	while (count <= delay*10) {
		rt_timer_spin(1000*100);
		count++;
	}
}


int rt_task_sleep_ms(unsigned long delay) {
	return rt_task_sleep(1000*1000*delay);
}

int main(int argc, char const *argv[])
{
	mlockall(MCL_CURRENT | MCL_FUTURE); /* lock all memory */

	RT_TASK_INFO curtaskinfo;

 	rt_print_auto_init(1);
 	
 	rt_sem_create(&semaphore, "sem_name", sem_count, MODE);/* create semaphore */
 	rt_sem_create(&start, "start_sem", 0, MODE);

 	// rt_mutex_create(&mutex, "mutex_name");

	rt_task_create(&medium_task, "medium_task", 0, M, T_CPU(0)); /* priority 25 */
	rt_task_create(&low_task, "low_task", 0, L, T_CPU(0)); /* priority 25 */
	rt_task_create(&high_task, "high_task", 0, H, T_CPU(0)); /* priority 25 */

	rt_task_start(&medium_task, &medium_task_func, NULL);
	rt_task_start(&low_task, &low_task_func, NULL);
	rt_task_start(&high_task, &high_task_func, NULL);

	rt_task_shadow(NULL, "task_main", 50, T_CPU(0));/* main task is task3 */

	rt_task_inquire(NULL,&curtaskinfo);
	// rt_task_sleep_ms(2000);
	rt_printf("Task %s broadcasts sem.\n", curtaskinfo.name);

	rt_sem_broadcast(&start);
	// rt_sem_broadcast(&semaphore);/* broadcast semaphore */
	rt_printf("Task %s done broadcasting sem.\n", curtaskinfo.name);

	rt_task_sleep_ms(1000);


	// while(1){

	// }
	// usleep(10000000);
	rt_sem_delete(&start);
	// rt_sem_delete(&semaphore);

	rt_printf("sem deleted.\n");


	return 0;
}
