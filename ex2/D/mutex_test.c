#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 2

int running = 1;
int var1 = 0;
int var2 = 0;

// create mutex
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

void* thread1_work(void* var)
{
		//~ int *var_arr = (int*)var;
		
		while ( running )
		{
			//~ pthread_mutex_lock( &mutex1 );
			var1++;
			var2 = var1;
			//~ pthread_mutex_unlock( &mutex1 );
		}
		
		return NULL;
}

void* thread2_work(void* var)
{
		//~ int *var_arr = (int*)var;
		
		for (int i = 1; i <= 20; i++)
		{
			//~ pthread_mutex_lock( &mutex1 );
			printf("Number 1 is %d, number 2 is %d \n", var1, var2);
			usleep(100000);
			//~ pthread_mutex_unlock( &mutex1 );
		}
		running = 0;
		
		return NULL;
}

int main( int argc, char** argv)
{
		//~ int var[] = {1, 2};
		
		pthread_t threads[NUM_THREADS];
		
		pthread_create( &threads[0], NULL, thread1_work, NULL);
		pthread_create( &threads[1], NULL, thread2_work, NULL);
		
		for (int i = 0; i < NUM_THREADS; i++)
		{
			pthread_join( threads[i], NULL);
			printf("thread nr %d finished\n", i);
		}
		
		printf("all threads finished");
		
}
