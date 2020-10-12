#ifndef EVALUATE_H
#define EVALUATE_H

#include "Board.h"

#define CONNECTEDPAWN   10
#define PHALANX         20
#define DOUBLEDPAWNMG   10
#define DOUBLEDPAWNEG   30
#define ISOLATEDPAWNMG  10
#define ISOLATEDPAWNEG  20
#define BACKWARDPAWNMG  8
#define BACKWARDPAWNEG  16
#define ROOKOPENFILEMG  20
#define ROOKOPENFILEEG  10
#define ROOKHALFFILEMG  10
#define ROOKHALFFILEEG  5
#define ROOKQUEENFILE   8
#define BISHOPPAIR      30
#define BADBISHOP       5
#define OUTPOST         10
#define KNIGHTPAIR      8
#define ROOKPAIR        16
#define RETURNINGBISHOP 30
#define TEMPO           10

int evaluate(BOARD *pos, char board[8][8], int color);

#endif