#ifndef EVALUATE_H
#define EVALUATE_H

#include "Board.h"

#define CONNECTEDPAWN   10
#define DOUBLEDPAWN     20
#define ISOLATEDPAWN    20
#define BACKWARDPAWN    8
#define ROOKOPENFILE    16
#define ROOKHALFFILE    8
#define ROOKQUEENFILE   7
#define BISHOPPAIR      30
#define BADBISHOP       5
#define OUTPOST         10
#define KNIGHTPAIR      8
#define ROOKPAIR        16
#define RETURNINGBISHOP 30 
#define TEMPO           10

int evaluate(BOARD *pos, char board[8][8], int color);

#endif