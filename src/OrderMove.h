#ifndef ORDERMOVE_H
#define ORDERMOVE_H

#include "Move.h"

#define BESTMOVE    100000000
#define HASHMOVE    90000000
#define WCAPTURE    80001200
#define PROMOTION   80001150
#define CASTLING    80001140
#define ECAPTURE    80000600
#define KILLER1     80000550
#define KILLER2     80000540
#define COUNTER     80000530
#define UNDERPROM   80000520
#define LCAPTURE    80000000

int wCapMove_score(char piece, char op_piece, char board[8][8], int x1, int y1, int x2, int y2);

int bCapMove_score(char piece, char op_piece, char board[8][8], int x1, int y1, int x2, int y2);

int quietMove_score(BOARD *pos, MOVE *move, int origin, int x, int y, int ply, int color);

int wCapQsearch_score(char piece, char op_piece);

int bCapQsearch_score(char piece, char op_piece);

int piece_value(char piece);

void movesort(MOVE moves[256], int sort[256], int length, int current);

void skipHashMove(MOVE moves[256], int sort[256], int length, MOVE *hash_move, int exist);

int orderMove_root(MOVE moves[256], int sort[256], int length, MOVE *best_move, MOVE *hash_move);

#endif