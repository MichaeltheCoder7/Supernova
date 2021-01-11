#ifndef CHECKMOVE_H
#define CHECKMOVE_H

#include "board.h"
#include "move.h"

int isWhitePiece(unsigned char piece);

int isBlackPiece(unsigned char piece);

int isWhitePiece_withKing(unsigned char piece);

int isBlackPiece_withKing(unsigned char piece);

int CheckCapture_wpawn(BOARD *pos, int new_x, int new_y);

int CheckCapture_bpawn(BOARD *pos, int new_x, int new_y);

int CheckMove_wkingside(BOARD *pos);

int CheckMove_wqueenside(BOARD *pos);

int CheckMove_bkingside(BOARD *pos);

int CheckMove_bqueenside(BOARD *pos);

int isPseudoLegal(BOARD *pos, MOVE *move, int color);

#endif