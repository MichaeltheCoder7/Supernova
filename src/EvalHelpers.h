#ifndef EVALHELPERS_H
#define EVALHELPERS_H

#include <stdbool.h>
#include "board.h"

int wpawn_controlled(unsigned char board[8][8], int x, int y);

int bpawn_controlled(unsigned char board[8][8], int x, int y);

bool candidate_passed_white(unsigned char board[8][8], int x, int y);

bool candidate_passed_black(unsigned char board[8][8], int x, int y);

bool connected_white(unsigned char board[8][8], int x, int y);

bool connected_black(unsigned char board[8][8], int x, int y);

bool phalanx_white(unsigned char board[8][8], int x, int y);

bool phalanx_black(unsigned char board[8][8], int x, int y);

bool isolated_white(unsigned char board[8][8], int y);

bool isolated_black(unsigned char board[8][8], int y);

bool backward_white(unsigned char board[8][8], int x, int y);

bool backward_black(unsigned char board[8][8], int x, int y);

bool openFile(unsigned char board[8][8], int y);

bool queenFile(BOARD *pos, int y);

bool semiOpenFile_white(unsigned char board[8][8], int y);

bool semiOpenFile_black(unsigned char board[8][8], int y);

bool outpost_white(unsigned char board[8][8], int x, int y);

bool outpost_black(unsigned char board[8][8], int x, int y);

bool badBishop_white(unsigned char board[8][8], int x, int y);

bool badBishop_black(unsigned char board[8][8], int x, int y);

int wking_file(unsigned char board[8][8], int y);

int bking_file(unsigned char board[8][8], int y);

#endif