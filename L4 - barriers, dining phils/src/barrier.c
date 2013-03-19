#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "barrier.h"

int barrier_init(barrier_t* barrier, int count) {
  int status;
  /** Initialize barrier threshold and counter to same value
      counter will decrement with each thread **/
  barrier->threshold = barrier->counter = count;
  /** Initialize pthread_mutex_t **/
  status = pthread_mutex_init(&barrier->lock, NULL);
  if(status != 0) return status;
  
  /** Initialize pthread_cond_t **/
  status = pthread_cond_init(&barrier->condition, NULL);
  if(status != 0) {
    /** If cond_init fails, clean up mutex_t **/
    pthread_mutex_destroy(&barrier->lock);
    return status;
  }
  
  /** Set barrier to valid**/
  barrier->valid = 1;
  return 0;
}

int barrier_destroy(barrier_t* barrier) {
  int status, cond_status;
  /** Only destory if the barrier is valid **/
  if(barrier->valid != 1) {
    return -1;
  }
  
  /** Lock the mutex so nothing happens while we're destroying **/
  status = pthread_mutex_lock(&barrier->lock);
  if(status != 0) return status;
  
  /** If there are threads waiting on this barrier, don't destory it **/
  if(barrier->counter != barrier->threshold) {
    pthread_mutex_unlock(&barrier->lock);
    return -1;
  }
  
  /** Set barrier to invalid **/
  barrier->valid = 0;
  status = pthread_mutex_unlock(&barrier->lock);
  if(status != 0) return status;
  
  /** Clean up mutex_t and cond_t **/
  status = pthread_mutex_destroy(&barrier->lock);
  cond_status = pthread_cond_destroy(&barrier->condition);
  
  return (status == 0) ? status : cond_status;
}

int barrier_wait(barrier_t* barrier) {
  int status, temp;
  /** Lock the mutex **/
  status = pthread_mutex_lock(&barrier->lock);
  if(status != 0) return status;
  /** Decrement the counter and test for barrier break **/
  if(--barrier->counter == 0) {
    /** Reset the counter **/
    barrier->counter = barrier->threshold;
    /** Release all threads held by the condition variable **/
    status = pthread_cond_broadcast(&barrier->condition);
    if(status == 0) status = -1;
  } else {
    /** Not enough threads have hit yet, wait on condition variable **/
    status = pthread_cond_wait(&barrier->condition, &barrier->lock);
  }
  /** Unlock mutex **/
  pthread_mutex_unlock(&barrier->lock);
  return status; /** if -1 something woke up **/
}

/** The threadID param is included for demonstriation purposes only
  * simply to keep track of the 10 threads coming through here.
  * Each thread is a assigned a color based on threadID, though the last
  * 3 threads will be white since there are only 8 colors available
**/
int barrier_wait_verbose(barrier_t* barrier, int threadID) {
  int status, temp;
  /** Lock the mutex **/
  status = pthread_mutex_lock(&barrier->lock);
  printf("\033[1;3%dmthread[%x] waiting for the barrier\033[0m\t(count = %d)\n", threadID, threadID, barrier->counter);
  if(status != 0) return status;
  /** Decrement the counter and test for barrier break **/
  if(--barrier->counter == 0) {
    /** Reset the counter **/
    barrier->counter = barrier->threshold;
    /** Release all threads held by the condition variable **/
    status = pthread_cond_broadcast(&barrier->condition);
    if(status == 0) status = -1;
  } else {
    /** Not enough threads have hit yet, wait on condition variable **/
    status = pthread_cond_wait(&barrier->condition, &barrier->lock);
  }
  printf("\033[1;3%dmthread[%x] through the barrier!\033[0m\t\t(count = %d)\n", threadID, threadID, barrier->counter);
  /** Unlock mutex **/
  pthread_mutex_unlock(&barrier->lock);
  return status; /** if -1 something woke up **/
}