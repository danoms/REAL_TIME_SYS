#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_PHILOSOPHERS 5

pthread_mutex_t forks[NUM_PHILOSOPHERS];
//~ pthread_mutex_t fork1, fork2, fork3, fork4, fork5;

void swap_if_larger( int* fork1, int* fork2)
{
		int temp;
		if (*fork1 > *fork2)
		{
			temp = *fork1;
			*fork1 = *fork2;
			*fork2 = temp;
		}
		
		return NULL;
}

void* eat( void* philosopher_num_p )
{
		
		int philosopher_num = (int)philosopher_num_p;
		
		int fork1 = philosopher_num;
		int fork2 = (philosopher_num + 1) % NUM_PHILOSOPHERS; 
		
		swap_if_larger( &fork1, &fork2);
		
	while(1)
	{
		pthread_mutex_lock( &forks[fork1] );
		printf("philosopher %d locked fork %d\n", philosopher_num, fork1);
		pthread_mutex_lock( &forks[fork2] );
		printf("philosopher %d locked fork %d\n", philosopher_num, fork2);
		
		printf("philosopher %d START eating!\n", philosopher_num);
		usleep(1000);
		printf("philosopher %d DONE eating!\n", philosopher_num);
		
		pthread_mutex_unlock( &forks[fork2] );
		printf("philosopher %d unlocked fork %d\n", philosopher_num, fork2);
		pthread_mutex_unlock( &forks[fork1] );
		printf("philosopher %d unlocked fork %d\n", philosopher_num, fork1);
	}
		return NULL;
}

int main( int argc, char** argv)
{
	// initialize mutex
		for (int i = 0; i < NUM_PHILOSOPHERS; i++)
		{	
			pthread_mutex_init( &forks[i], NULL);
		}

		pthread_t threads[NUM_PHILOSOPHERS];
		
	// create threads
		for (int i = 0; i < NUM_PHILOSOPHERS; i++)
		{
			pthread_create( &threads[i], NULL, eat, i);
			printf("Philosopher nr %d born!\n", i);
		}
		
	// finish threads
		for (int i = 0; i < NUM_PHILOSOPHERS; i++)
		{
			pthread_join( threads[i], NULL);
			printf("Philosopher nr %d finished!\n", i);
		}
	
		printf("All philosophers eaten once!\n");
		
		return 0;
}
