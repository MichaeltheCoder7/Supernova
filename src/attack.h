#ifndef ATTACK_H
#define ATTACK_H

#include "board.h"

int ifCheck(BOARD *pos, int color);

int isThreatened(unsigned char board[8][8], int x, int y, int color);

#endif