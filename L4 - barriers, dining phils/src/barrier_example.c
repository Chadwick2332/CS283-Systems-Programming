#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "barrier.h"

/** Demonstration thread_struct definition **/
typedef struct thread_struct {
  barrier_t* barrier;     /** reference to the barrier **/
  int threadID;           /** ID [0, 10] for each thread **/
} t_args;

void* thread_routine(void* arg) {
	t_args* args = (t_args*) arg;
	barrier_t* barrier = args->barrier;
	sleep(5);
	barrier_wait_verbose(barrier, args->threadID);
	pthread_exit(0);
}

int main (int argc, char const *argv[])
{
	int i;
	barrier_t barrier;
	barrier_init(&barrier, 5);
	barrier_t* barrier_pointer = &barrier;
	pthread_t threads[10];
	t_args* args = (t_args*) calloc(10, sizeof(t_args));
	/** Construct thread argument structs **/
	for(i = 0; i < 10; i++) {
		args[i].barrier = barrier_pointer;
		args[i].threadID = i;
	}
	/** Initializing threads **/
	for(i = 0; i < 10; i++) {
		if(pthread_create(&threads[i], NULL, thread_routine, &args[i])) {
			fprintf(stderr, "Thread could not be spawned. Aborting.");
			exit(1);
		}
	}
	/** Wait for threads to terminate before exit **/
	for(i = 0; i < 10; i++) {
		pthread_join(threads[i], NULL);
	}
	
	/** Destroy barrier **/
	barrier_destroy(&barrier);
	return 0;
}