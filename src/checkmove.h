#ifndef CHECKMOVE_H
#define CHECKMOVE_H

#include "board.h"
#include "move.h"

int isWhitePiece(unsigned char piece);

int isBlackPiece(unsigned char piece);

int isWhitePiece_withKing(unsigned char piece);

int isBlackPiece_withKing(unsigned char piece);

int checkCapture_wpawn(BOARD *pos, int new_x, int new_y);

int checkCapture_bpawn(BOARD *pos, int new_x, int new_y);

int checkMove_wkingside(BOARD *pos);

int checkMove_wqueenside(BOARD *pos);

int checkMove_bkingside(BOARD *pos);

int checkMove_bqueenside(BOARD *pos);

int isPseudoLegal(BOARD *pos, MOVE *move, int color);

#endif