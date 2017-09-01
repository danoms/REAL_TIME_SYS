#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#include <errno.h>

#define RESOURCE_COUNT 3
#define NUM_THREADS 5

// memory for semaphore
sem_t s;

void* some_work(void* value_p)
{
	//~ int unsigned long thread_nr = *(pthread_t*)value_p;
	int thread_nr = value_p;
		for ( int i = 0; i < NUM_THREADS; i++ )
			{
				
				sem_wait( &s );
				printf("Thread number %d prints i = %d\n", thread_nr, i);
				usleep(100000);
				sem_post( &s );
			}
		return NULL;
}

int main( int argc, char** argv)
{	
			int some_test_value = 10;
		
			pthread_t pthreads[NUM_THREADS];
			
			//int sem_init( sem_t *sp, int type, unsigned int count)
			if (sem_init( &s, NULL, RESOURCE_COUNT) != 0 )
				perror("semaphor");
			
			// create threads one by one
			for ( int i = 0; i < NUM_THREADS; i++)
			{
				//pthread_t *thread, int type, void* func, void* arg_p
				pthread_create( &pthreads[i], NULL, some_work, i);
			}
			
			// wait for threads to complete	
			for (int i = 0; i < NUM_THREADS; i++ )
			{
				// pthread, retval
				pthread_join( pthreads[i], NULL );
				printf("Pthread nr %d finished job\n", i);
			}
			
			printf("all finished!!\n");
			
			return 0;
}
