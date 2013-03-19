//
//  game-lib.c
//  pipes
//
//  Created by Christian Benincasa on 3/4/13.
//  Copyright (c) 2013 Christian Benincasa. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game-lib.h"

const int DIR_LEFT = 0;
const int DIR_RIGHT = 1;

void generate_board(Board** board, int dimension) {
	int i, j;
  *board = (Board*) malloc(sizeof(Board));
  (*board)->dimension = dimension;
  (*board)->finished = 0;
  (*board)->rows = (Row*) malloc(sizeof(Row) * dimension);
  for (i = 0; i < dimension; i++) {
    (*board)->rows[i].squares = (int*) malloc(sizeof(int) * dimension);
    for(j = 0; j < dimension; j++) {
      (*board)->rows[i].squares[j] = 0;
    }
  }
}

void dealloc(Board* board) {
  int i;
  for (i = 0; i < board->dimension; i++) {
    free(board->rows[i].squares);
  }
  free(board->rows);
  free(board);
}

GameMove* placePieceAtPosition(Board* board, int pieceValue, int row, int col) {
  if((row < 0 || row > board->dimension) || (col < 0 || col > board->dimension)) return NULL;
  if(pieceValue < 0 || pieceValue > 2) return NULL;
  int topRow = findFirstOpenPosition(board, col);
  if (topRow == -1) {
    return NULL;
  }
  board->rows[topRow].squares[col] = pieceValue;
  if(pieceValue == 1) {
    //printf("Parent moves to (%d, %d)\n", board->dimension - i - 1, col);
  } else {
    //printf("Child moves to (%d, %d)\n", board->dimension - i - 1, col);
  }
  GameMove* move = (GameMove*) malloc(sizeof(GameMove));
  move->row = topRow;
  move->col = col;
  return move;
}

GameMove* placePieceInColumn(Board* board, int pieceValue, int col) {
  return placePieceAtPosition(board, pieceValue, 0, col);
}

GameMove* findPieceAtBestPosition(Board* board, int pieceValue) {
  srand((unsigned int) time(NULL));
  int best_row = board->dimension - 1, best_col = rand() % (board->dimension);
  int current_lowest_moves = 4;
  int i, j, k;
  for (i = 0; i < board->dimension; i++) {
    int firstOpen = findFirstOpenPosition(board, i);
    if(firstOpen != (board->dimension - 1)) {
      int minimum_moves = 4;
      //look down
      for(j = firstOpen; j < board->dimension; j++) {
        if(board->rows[j].squares[i] == pieceValue) {
          minimum_moves--;
        } else {
          break;
        }
      }
      if(minimum_moves < current_lowest_moves) {
        current_lowest_moves = minimum_moves;
        best_row = j;
        best_col = i;
      }
      // down - right
      minimum_moves = 4;
      for (j = firstOpen, k = i;
           j < board->dimension && k < board->dimension;
           j++, k++) {
        if(board->rows[j].squares[k] == pieceValue) {
          minimum_moves--;
        } else {
          break;
        }
      }
      if(minimum_moves < current_lowest_moves) {
        current_lowest_moves = minimum_moves;
        best_row = j;
        best_col = i;
      }
      // down - left
      minimum_moves = 4;
      for (j = firstOpen, k = i;
           j < board->dimension && k > -1;
           j++, k--) {
        if(board->rows[j].squares[k] == pieceValue) {
          minimum_moves--;
        } else {
          break;
        }
      }
      if(minimum_moves < current_lowest_moves) {
        current_lowest_moves = minimum_moves;
        best_row = j;
        best_col = i;
      }
    }
    if(i != 0) {
      int minimum_moves = 4;
      //look left
      for(j = i - 1; j > -1; j--) {
        if(board->rows[firstOpen].squares[j] == pieceValue) {
          minimum_moves--;
        } else {
          break;
        }
      }
      if(minimum_moves < current_lowest_moves) {
        current_lowest_moves = minimum_moves;
        best_row = firstOpen;
        best_col = i;
      }
      
    }
    if(i != board->dimension - 1) {
      int minimum_moves = 4;
      //look right
      for(j = i + 1; j < board->dimension; j++) {
        if(board->rows[firstOpen].squares[j] == pieceValue) {
          minimum_moves--;
        } else {
          break;
        }
      }
      if(minimum_moves < current_lowest_moves) {
        current_lowest_moves = minimum_moves;
        best_row = firstOpen;
        best_col = i;
      }
    }
  }
  GameMove* bestMove = (GameMove*) malloc(sizeof(GameMove));
  bestMove->row = best_row;
  bestMove->col = best_col;
  return bestMove;
}

GameMove* placePieceAtBestPosition(Board* board, int pieceValue) {
  srand((unsigned int) time(NULL));
  int best_row = rand() % (board->dimension), best_col = rand() % (board->dimension);
  int current_lowest_moves = 4;
  int highest_piece_sequence = 0, current_piece_sequence;
  int i, j, k;
  for (i = 0; i < board->dimension; i++) {
    int firstOpen = findFirstOpenPosition(board, i);
    if(firstOpen != (board->dimension - 1)) {
      int minimum_moves = 4;
      //look down
      for(j = firstOpen; j < board->dimension; j++) {
        if(board->rows[j].squares[i] == pieceValue) {
          minimum_moves--;
        } else {
          break;
        }
      }
      if(minimum_moves < current_lowest_moves) {
        current_lowest_moves = minimum_moves;
        best_row = j;
        best_col = i;
      }
      // down - right
      minimum_moves = 4;
      for (j = firstOpen, k = i;
           j < board->dimension && k < board->dimension;
           j++, k++) {
        if(board->rows[j].squares[k] == pieceValue) {
          minimum_moves--;
        } else {
          break;
        }
      }
      if(minimum_moves < current_lowest_moves) {
        current_lowest_moves = minimum_moves;
        best_row = j;
        best_col = i;
      }
      // down - left
      minimum_moves = 4;
      for (j = firstOpen, k = i;
           j < board->dimension && k > -1;
           j++, k--) {
        if(board->rows[j].squares[k] == pieceValue) {
          minimum_moves--;
        } else {
          break;
        }
      }
      if(minimum_moves < current_lowest_moves) {
        current_lowest_moves = minimum_moves;
        best_row = j;
        best_col = i;
      }
    }
    if(i != 0) {
      int minimum_moves = 4;
      //look left
      for(j = i - 1; j > -1; j--) {
        if(board->rows[firstOpen].squares[j] == pieceValue) {
          minimum_moves--;
        } else {
          break;
        }
      }
      if(minimum_moves < current_lowest_moves) {
        current_lowest_moves = minimum_moves;
        best_row = firstOpen;
        best_col = i;
      }
      
    }
    if(i != board->dimension - 1) {
      int minimum_moves = 4;
      //look right
      for(j = i + 1; j < board->dimension; j++) {
        if(board->rows[firstOpen].squares[j] == pieceValue) {
          minimum_moves--;
        } else {
          break;
        }
      }
      if(minimum_moves < current_lowest_moves) {
        current_lowest_moves = minimum_moves;
        best_row = firstOpen;
        best_col = i;
      }
    }
  }
  // This is experimental right now...finding best move
  // of opponent and placing a piece there. Still some kinks to be
  // worked out.
//  if (current_lowest_moves < 2) {
//    GameMove* move;
//    if (pieceValue == 1) {
//      move = findPieceAtBestPosition(board, 2);
//    }
//    else {
//      move = findPieceAtBestPosition(board, 1);
//    }
//    best_col = move->col;
//    best_row = move->row;
//  }
  return placePieceAtPosition(board, pieceValue, best_row, best_col);
}

int checkForWin(Board* board, int pieceValue) {
  int matches = 0, topPiece, i, win = 0;
  for (i = 0; i < board->dimension; i++) {
    topPiece = findTopPiecePosition(board, i);
    if (board->rows[topPiece].squares[i] == 0) {
      continue;
    } else {
      if(i >= 3) {
        // look left
        matches = searchHorizontally(board, pieceValue, DIR_LEFT, topPiece, i);
        if(matches == 4) return 1;
        matches = 0;
        // down - left
        if(topPiece <= (board->dimension - 1) - 3) {
          matches = searchDiagonally(board, pieceValue, DIR_LEFT, topPiece, i);
          if(matches == 4) return 1;
          matches = 0;
        }
        // down - right
        if(topPiece != board->dimension - 1) {
          matches = searchDiagonally(board, pieceValue, DIR_RIGHT, topPiece, i);
          if(matches == 4) return 1;
          matches = 0;
        }
      }
      // right-most column
      if(i <= (board->dimension - 1) - 3) {
        // look right
        matches = searchHorizontally(board, pieceValue, DIR_RIGHT, topPiece, i);
        if(matches == 4) return 1;
        matches = 0;
        // up - right
        if(topPiece != 0) {
        }
        //down - right
        if(topPiece != board->dimension - 1) {
        }
      }
      if(topPiece <= (board->dimension - 1)) {
        matches = searchDown(board, pieceValue, topPiece, i);
        if(matches == 4) return 1;
        matches = 0;
      }
    }
  }
  win = matches;
  return win;
}

// Return length of piece sequence
int searchHorizontally(Board* board, int search, int direction, int row, int col) {
  int matches = 0, j;
  // search left
  if(direction == DIR_LEFT) {
    for(j = col; j > -1; j--) {
      if(board->rows[row].squares[j] == search) {
        matches++;
      } else {
        break;
      }
    }
  }
  // search right
  else if(direction == DIR_RIGHT) {
    for(j = col; j < board->dimension; j++) {
      if(board->rows[row].squares[j] == search) {
        matches++;
      } else {
        break;
      }
    }
  }
  return matches;
}

// Search diagonally left-down or right-down.
int searchDiagonally(Board* board, int search, int diretion, int row, int col) {
  int matches = 0, j, k;
  if(diretion == DIR_LEFT) {
    for (j = row, k = col;
         j < board->dimension && k > -1;
         j++, k--) {
      if(board->rows[j].squares[k] == search) {
        matches++;
      } else {
        break;
      }
    }
  }
  else if(diretion == DIR_RIGHT) {
    for (j = row, k = col;
         j < board->dimension && k < board->dimension;
         j++, k++) {
      if(board->rows[j].squares[k] == search) {
        matches++;
      } else {
        break;
      }
    }
  }
  
  return matches;
}

// Search down. no need to search up.
int searchDown(Board* board, int search, int row, int col) {
  int j;
  int matches = 0;
  for(j = row; j < board->dimension; j++) {
    if(board->rows[j].squares[col] == search) {
      matches++;
    } else {
      break;
    }
  }
  return matches;
}

// Finds position to topmost empty space
int findFirstOpenPosition(Board* board, int col) {
  int i = 0;
  for(i = board->dimension - 1; i > -1; i--) {
    if(board->rows[i].squares[col] == 0) break;
  }
  return i;
}

// Similar to above function, but returns position of topmost piece
int findTopPiecePosition(Board* board, int col) {
  int i;
  for(i = 0; i < board->dimension; i++) {
    if(board->rows[i].squares[col] != 0) break;
  }
	i = (i == board->dimension) ? i - 1 : i;
  return i;
}

// Print out the board
// Parent pieces in blue
// Child pieces in Red
void printBoard(Board* board) {
  int i, j;
  for(i = 0; i < board->dimension; i++) {
    printf("%d   ", board->dimension - 1 - i);
    for(j = 0; j < board->dimension; j++) {
      switch (board->rows[i].squares[j]) {
        case 0:
          printf("- ");
          break;
        case 1:
          printf("\033[1;34mX \033[0m");
          break;
        case 2:
          printf("\033[1;31mX \033[0m");
          break;
        default:
          break;
      }
    }
    printf("\n");
  }
  printf("    ");
  for (i = 0; i < board->dimension; i++) {
    printf("%d ", i);
  }
  printf("\n\n");
}