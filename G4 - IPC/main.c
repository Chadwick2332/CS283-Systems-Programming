#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <semaphore.h>
#include <signal.h>
#include "game-lib.h"

// Kill the zombies!
void reaper(int s) {
	int status = s;
	//while(wait(&status) > 0) continue;
	printf("Killed zombie!\n");
}

int rand_lim(int limit);

int main (int argc, char const *argv[])
{
  // Dimension of the boards and number of children to spawn
	int dimension = 8, children = 2, i, j;
  char* env_dimension = getenv("DIMENSION");
  if(env_dimension != NULL) {
    dimension = atoi(env_dimension);
  }
  char* env_children = getenv("GAMES");
  if(env_children != NULL) {
    children = atoi(env_children);
  }
  // Flag for child intelligence
  // Right now, the games are pretty boring
  // if the child is intelligent (haven't implemented competitive logic)
  int child_play_to_win = 0;
  // Multi-dimensional array of file descriptors for pipes
	int fd[children][2][2];
  // Array of child process ids
	pid_t childpid[children];
  // Array of Boards
	Board** boards = (Board**) malloc(sizeof(Board*) * children);
  // Handle child exiting
	//signal(SIGCHLD, reaper);
  // Only need one move, moves happen sequentially.
  GameMove* lastMove;
  // A char buffer used for reading and writing a GameMove through pipes
  char buf[80];
  
	for(i = 0; i < children; i++) {
		generate_board(&(boards[i]), dimension);
    
		pipe(fd[i][0]);
		/** 
      child read     = fd[i][0][0]
      parent write   = fd[i][0][1]
    **/
    
		pipe(fd[i][1]);
    
		/**
      parent read    = fd[i][1][0]
      child write    = fd[i][1][1]
    **/
    
    childpid[i] = fork();
    
		if(childpid[i] == -1) {
			perror("fork");
			exit(1);
		}
		if(childpid[i] == 0) {
      srand(getpid());
      for (j = 0; j <=i; j++) {
        // close all parent reads
        close(fd[j][0][1]);
        // close all parent writes
        close(fd[j][1][0]);
      }
      long nbytes;
      while(!boards[i]->finished) {
        // Read in move from parent
        nbytes = read(fd[i][0][0], &buf, sizeof(GameMove));
        if(nbytes <= 0) {
          break;
        }
        // cast move to GameMove*
        lastMove = (GameMove*) buf;
        // place parent piece at the position read in
        // Because of the while loops in the parent and child processes, this move
        // can always be considered safe and non-NULL
        lastMove = placePieceAtPosition(boards[i], 1, lastMove->row, lastMove->col);
        // Check for a win. If there is a win, this will return 1
        // Else, child will make a move
        if(!(boards[i]->finished = checkForWin(boards[i], 1))) {
          // If child is playing to win, like the parent, use a different
          // function here.
          if(child_play_to_win) {
            // NULL case handles in best position function
            lastMove = placePieceAtBestPosition(boards[i], 2);
          } else {
            lastMove = placePieceInColumn(boards[i], 2, rand() % boards[i]->dimension);
          }
          // After child makes their move, check for a child win again.
          boards[i]->finished = checkForWin(boards[i], 2);
          // write move back to parent
          write(fd[i][1][1], lastMove, sizeof(GameMove));
          // If the board is finished (there was a win),
          if (boards[i]->finished) {
            // Child wins
            // close child read fd
            close(fd[i][0][0]);
            // close child write fd
            close(fd[i][1][1]);
            break;
          }
        } else {
          // Parent won
          write(fd[i][1][1], lastMove, sizeof(GameMove));
          // close child read fd
          close(fd[i][0][0]);
          // close child write fd
          close(fd[i][1][1]);
          break;
        }
      }
      dealloc(boards[i]);
			exit(0);
		}
	}
	
  // When this hits children amount, all games are done
  int games_complete = 0, error = 0;
  long nbytes;
  GameMove* tmpMove;
  // Make first move to all children
  for (i = 0; i < children; i++) {
    close(fd[i][0][0]);
    close(fd[i][1][1]);
    lastMove = placePieceAtBestPosition(boards[i], 1);
    write(fd[i][0][1], lastMove, sizeof(GameMove));
  }
  while (games_complete != children && !error) {
    for (i = 0; i < children; i++) {
      // Read move from child
      if(!boards[i]->finished) {
        nbytes = read(fd[i][1][0], &buf, sizeof(GameMove));
        if(nbytes < 0) {
          printf("\033[1;32mRead child[%d] error in parent.\033[0m\n\n", childpid[i]);
          error = 1;
          exit(1);
        }
        else if(nbytes == 0) {
          games_complete++;
          break;
        }
        // Cast child move to GameMove*
        tmpMove = (GameMove*) buf;
        if(tmpMove->row == lastMove->row && tmpMove->col == lastMove->col) {
          kill(childpid[i], SIGKILL);
        }
        // place child piece
        placePieceAtPosition(boards[i], 2, lastMove->row, lastMove->col);
        // Check for a child win...
        boards[i]->finished = checkForWin(boards[i], 2);
        if (!boards[i]->finished) {
          // No win yet, place piece at best position
          lastMove = placePieceAtBestPosition(boards[i], 1);
          // check for win again
          boards[i]->finished = checkForWin(boards[i], 1);
          // Write move back to child
          write(fd[i][0][1], lastMove, sizeof(GameMove));
          // If we won, close everything up and increment
          // the games_complete counter.
          if(boards[i]->finished) {
            // close parent read fd
            close(fd[i][1][0]);
            // close parent write fd
            close(fd[i][0][1]);
            printf("\033[1;3%dmParent wins against child[%d]!\033[0m\n", i + 1, childpid[i]);
            games_complete++;
            printBoard(boards[i]);
          }
        } else {
          printf("Parent wins against child[%d]!\n", childpid[i]);
          // write winning move back to child
          write(fd[i][0][1], lastMove, sizeof(GameMove));
          // close parent write fd
          close(fd[i][0][1]);
          // close parent read fd
          close(fd[i][1][0]);
          games_complete++;
          printBoard(boards[i]);
        }
      }
    }
  }
  
  for(i = 0; i < children; i++) {
    waitpid(childpid[i], NULL, 0);
  }
  
	/** Cleanup **/
	for(i = 0; i < children; i++) {
		dealloc(boards[i]);
	}
	free(boards);
  //free(tmpMove);
	free(lastMove);
	return 0;
}

int rand_lim(int limit) {  
  int divisor = RAND_MAX/(limit+1);
  int retval;
  
  do {
    retval = rand() / divisor;
  } while (retval > limit);
  
  return retval;
}