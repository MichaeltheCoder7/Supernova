#ifndef EVALHELPERS_H 
#define EVALHELPERS_H

#include <stdbool.h>
#include "Board.h"

int wpawn_controlled(char board[8][8], int x, int y);

int bpawn_controlled(char board[8][8], int x, int y);

bool candidate_passed_white(char board[8][8], int x, int y);

bool candidate_passed_black(char board[8][8], int x, int y);

bool connected_white(char board[8][8], int x, int y);

bool connected_black(char board[8][8], int x, int y);

bool phalanx_white(char board[8][8], int x, int y);

bool phalanx_black(char board[8][8], int x, int y);

bool isolated_white(char board[8][8], int y);

bool isolated_black(char board[8][8], int y);

bool backward_white(char board[8][8], int x, int y);

bool backward_black(char board[8][8], int x, int y);

bool openFile(char board[8][8], int y);

bool queenFile(BOARD *pos, int y);

bool semiOpenFile_white(char board[8][8], int y);

bool semiOpenFile_black(char board[8][8], int y);

bool outpost_white(char board[8][8], int x, int y);

bool outpost_black(char board[8][8], int x, int y);

bool badBishop_white(char board[8][8], int x, int y);

bool badBishop_black(char board[8][8], int x, int y);

int wking_file(char board[8][8], int y);

int bking_file(char board[8][8], int y);

#endif