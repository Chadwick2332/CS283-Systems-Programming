//
//  game-lib.h
//  pipes
//
//  Created by Christian Benincasa on 3/4/13.
//  Copyright (c) 2013 Christian Benincasa. All rights reserved.
//

#ifndef pipes_game_lib_h
#define pipes_game_lib_h

extern const int DIR_LEFT;
extern const int DIR_RIGHT;

typedef struct square_struct {
  int value;
} Square;

typedef struct row_struct {
  int* squares;
} Row;

typedef struct board_struct {
  int finished;
	int dimension;
  Row* rows;
} Board;

typedef struct game_move {
  int row;
  int col;
} GameMove;

void generate_board(Board** board, int dimension);
//Board* generate_board(int dimension);
void dealloc(Board* board);
void printBoard(Board* board);
GameMove* placePieceAtPosition(Board* board, int pieceValue, int x, int y);
GameMove* placePieceAtBestPosition(Board* board, int pieceValue);
GameMove* placePieceInColumn(Board* board, int pieceValue, int col);
int findFirstOpenPosition(Board* board, int col);
int findTopPiecePosition(Board* board, int col);
int checkForWin(Board* board, int pieceValue);
int searchHorizontally(Board* board, int search, int direction, int row, int col);
int searchDown(Board* board, int pieceValue, int row, int col);
int searchDiagonally(Board* board, int search, int diretion, int row, int col);

#endif
