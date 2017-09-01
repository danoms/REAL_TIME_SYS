#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

#define NUM_THREADS 2

int some_int_global = 0;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

void* print_value(void* value_p)
{
	//~ pthread_mutex_lock( &mutex1 );
		int* passed_value_p = value_p;
		
		some_int_global++;
		(*passed_value_p)++;
		printf("Hello world it's me thread printing value %d and global %d\n", *passed_value_p, some_int_global);
	//~ pthread_mutex_unlock( &mutex1 );
		return NULL;
}

int main(int argc, char* argv[])
{
		int some_int_local = 0;
		
		
		
		pthread_t threads[NUM_THREADS];
		int thread_args[NUM_THREADS];
		int result_code;
		unsigned index;
		
		//create all threads one by one
		for (index=0; index<NUM_THREADS; index++)
		{
			thread_args[index] = index;
			printf("In main: creating thread %d\n", index);
			result_code = pthread_create( &threads[index], NULL, print_value,
													&some_int_local);
			assert(!result_code);
		}
		
		//wait for each thread to complete
		for (index = 0; index < NUM_THREADS; index++ )
		{
			result_code = pthread_join( threads[ index ], NULL);
			assert( !result_code );
			printf( "In main: threaad %d has completed\n", index );			
		}
		
		printf( "In main: All threads completed succesfully\n global = %d, local = %d \n", some_int_global, some_int_local);
		exit(  EXIT_SUCCESS);
}
