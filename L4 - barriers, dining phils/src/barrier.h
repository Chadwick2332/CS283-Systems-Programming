#ifndef THREAD_BARRIER_H
#define THREAD_BARRIER_H
#include <pthread.h>

typedef struct barrier_struct {
  pthread_mutex_t lock;     /** mutex lock for operations **/
  pthread_cond_t condition; /** condition var for barrier waiting **/
  unsigned int threshold;   /** how many threads to wait for **/
  unsigned int counter;     /** how many threads left to wait for **/
  unsigned int valid;       /** can this barrier be used? **/
} barrier_t;

int barrier_init(barrier_t* barrier, int count);
int barrier_destroy(barrier_t* barrier);
int barrier_wait(barrier_t* barrier);
int barrier_wait_verbose(barrier_t* barrier, int threadID);

#endif