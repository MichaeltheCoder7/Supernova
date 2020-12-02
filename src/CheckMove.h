#ifndef CHECKMOVE_H 
#define CHECKMOVE_H

#include "Board.h"
#include "Move.h"

int isWhitePiece(char piece);

int isBlackPiece(char piece);

int isWhitePiece_withKing(char piece);

int isBlackPiece_withKing(char piece);

int CheckCapture_wpawn(BOARD *pos, int new_x, int new_y);

int CheckCapture_bpawn(BOARD *pos, int new_x, int new_y);

int CheckMove_wkingside(BOARD *pos);

int CheckMove_wqueenside(BOARD *pos);

int CheckMove_bkingside(BOARD *pos);

int CheckMove_bqueenside(BOARD *pos);

int isPseudoLegal(BOARD *pos, MOVE *move, int color);

#endif