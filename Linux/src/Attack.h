#ifndef ATTACK_H
#define ATTACK_H

#include "Board.h"

int ifCheck(BOARD *pos, int color);

int isThreatened(char board[8][8], int x, int y, int color);

#endif