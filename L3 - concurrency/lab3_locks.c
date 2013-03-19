#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
	pthread_mutex_t mutex;
	unsigned long* counter;
} thread_args;

void* thread_routine(void* arg);

int main (int argc, char const *argv[])
{
	int i = 0;
	unsigned long counter = 0;
	pthread_t threads[1000];
	thread_args* args = (thread_args*) malloc(sizeof(thread_args));
	pthread_mutex_init(&(args->mutex), NULL);
	args->counter = &counter;
	
	for(; i < 1000; i++) {
		pthread_create(&threads[i], NULL, thread_routine, args);
	}
	
	for(i = 0; i < 1000; i++) {
		pthread_join(threads[i], NULL);
	}
	
	printf("count: %ld\n", counter);
	pthread_mutex_destroy(&(args->mutex));
	free(args);
	return 0;
}

void* thread_routine(void* arg) {
	thread_args* args = (thread_args*) arg;
	unsigned long* count = args->counter;
	int i;

	for(i = 0; i < 1000; i++) {
		pthread_mutex_lock(&(args->mutex));
		(*count)++;
		pthread_mutex_unlock(&(args->mutex));
	}
	
	pthread_exit(0);
}