#ifndef CAPTUREGEN_H
#define CAPTUREGEN_H

#include "Board.h"
#include "Move.h"

int captureGen(BOARD *pos, MOVE all_moves[256], int sort[256], int color);

#endif