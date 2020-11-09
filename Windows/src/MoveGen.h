#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "Board.h"
#include "Move.h"

extern const int knight_moves_x[8];
extern const int knight_moves_y[8];
extern const int king_moves_x[8];
extern const int king_moves_y[8];

int moveGen(BOARD *pos, MOVE all_moves[256], int sort[256], int ply, int color);

#endif