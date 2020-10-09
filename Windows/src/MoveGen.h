#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "Board.h"
#include "Move.h"

int moveGen(BOARD *pos, MOVE all_moves[256], int sort[256], int ply, int color);

int captureGen(BOARD *pos, MOVE all_moves[256], int sort[100], int color);

#endif