#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "Board.h"

int moveGen_root(BOARD *pos, char all_moves[256][6], int sort[256], char best_move[6], char hash_move[6], int color);

int moveGen(BOARD *pos, char all_moves[256][6], int sort[256], char hash_move[6], int ply, int color);

int captureGen(BOARD *pos, char all_moves[100][6], int sort[100], int color);

int SEE(char board[8][8], char location[3], int target, int color);

#endif