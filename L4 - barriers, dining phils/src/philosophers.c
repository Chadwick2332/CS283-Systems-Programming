#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
 
typedef struct phil_strut {
  pthread_mutex_t* fork_left, *fork_right;
  char* name;
  pthread_t thread;
  int	success;
	int color;
} philosopher_t;
 
/* I know. */
int running = 1;
 
void *PhilPhunction(void *p) {
    philosopher_t *phil = (philosopher_t*) p;
    int status;
    int tries_left;
    pthread_mutex_t* fork_left, *fork_right, *fork_tmp;
 
    while (running) {
      /* Each philosopher starts off thinking */
      printf("\033[1;%dm%s is thinking\033[0m\n", phil->color, phil->name);
      /* Each philosopher pauses and thinks for a random amount of time [1, 8] */
      sleep( 1 + rand() % 8);
      fork_left = phil->fork_left;
      fork_right = phil->fork_right;
      /* Philosopher is hungry (will begin trying to grab some forks) */
      printf("\033[1;%dm%s is hungry\033[0m\n", phil->color, phil->name);
      tries_left = 2;
      do {
        /* Wait on the left fork */        
        status = pthread_mutex_lock(fork_left);
        /* Soft try on the right fork twice, then block */
        status = (tries_left > 0) ? pthread_mutex_trylock(fork_right) : pthread_mutex_lock(fork_right);
        if (status) {
          /* Right fork not ready, put down left fork */            
          pthread_mutex_unlock(fork_left);
          /* Switch forks (will hold on opposite fork and soft wait on other) */ 
          fork_tmp = fork_left;
          fork_left = fork_right;
          fork_right = fork_tmp;
          --tries_left;
        }
      } while(status && running);

      /* Philosopher successfully grabbed both forks and is eating */
      if (!status) {
          printf("\033[1;%dm%s is eating\033[0m\n", phil->color, phil->name);
          /* Eat for random amount of time */
          sleep(1 + rand() % 8);
          /* Put both forks down */
          pthread_mutex_unlock(fork_right);
          pthread_mutex_unlock(fork_left);
          printf("\033[1;%dm%s stopped eating\033[0m\n", phil->color, phil->name);
      }
    }
    pthread_exit(0);
}
 
int main(int argc, const char** argv) {
  const char* names[] = 
    {"Francis Bacon", "Kierkegaard", "Socrates", "Confucius", "John Locke" };
  pthread_mutex_t forks[5];
  philosopher_t philosophers[5];
  philosopher_t* phil;
  int i;
  int status;

  /* Initialize the mutexes for each philosopher */
  for (i = 0; i < 5; i++) {
      status = pthread_mutex_init(&forks[i], NULL);
      if (status) {
          printf("Failed to initialize mutexes.");
          exit(1);
      }
  }
 
  /* Initialize each philosopher
   * Zeroth philosopher is northmost when looking from a bird's eye view
   */
  for (i = 0; i < 5; i++) {
    /* Point to next philosopher in list */    
    phil = &philosophers[i];
    /* Choose i-th name from the list */
    phil->name = (char*) malloc(strlen(names[i]) + 1);
    strncpy(phil->name, names[i], strlen(names[i]));
    /* Assign forks to philosopher mutexes.
     * Zeroth fork starts to right of zeroth philosopher
     * Forks assigned clockwise when looking from a bird's eye view
     */
    phil->fork_left = &forks[(i + 1) % 5];
    phil->fork_right = &forks[i];
    /* ASCII Escape color code to differentiae philosopher */
		phil->color = 31 + i;
    /* Spawn the thread and save the return value in the philosopher */
    phil->success = pthread_create(&phil->thread, NULL, PhilPhunction, phil);
  }

  /* Let the philosophers go for 40 seconds */
  sleep(40);
  running = 0;
  printf("Cleaning up...\n");

  /* Wait for all of the philosophers to join back up */
  for(i = 0; i < 5; i++) {
      phil = &philosophers[i];
      if (!phil->success && pthread_join( phil->thread, NULL) ) {
          printf("error joining thread for %s", phil->name);
          exit(1);
      }
  }
  return 0;
}
