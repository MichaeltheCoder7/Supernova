#ifndef CHECKMOVE_H
#define CHECKMOVE_H

#include "Board.h"

int CheckCapture_wpawn(BOARD *pos, int new_x, int new_y);

int CheckCapture_bpawn(BOARD *pos, int new_x, int new_y);

int CheckMove_wkingside(BOARD *pos);

int CheckMove_wqueenside(BOARD *pos);

int CheckMove_bkingside(BOARD *pos);

int CheckMove_bqueenside(BOARD *pos);

#endif