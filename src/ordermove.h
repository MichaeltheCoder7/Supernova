#ifndef ORDERMOVE_H
#define ORDERMOVE_H

#include "move.h"
#include "thread.h"

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

static const int piece_value[13] = { 100, 325, 335, 500, 975, 20000, 100, 325, 335, 500, 975, 20000, 100 };
static const int lva_score[12] = { 5, 4, 3, 2, 1, 0, 5, 4, 3, 2, 1, 0 };
static const int mvv_score[13] = { 100, 200, 300, 400, 500, 0, 100, 200, 300, 400, 500, 0, 100 }; // empty square is worth 100 for en passant

int capMove_score(unsigned char piece, unsigned char op_piece, unsigned char board[8][8], int x1, int y1, int x2, int y2, int color);

int quietMove_score(THREAD *thread, BOARD *pos, MOVE *move, int origin, int x, int y, int ply, int color);

int capQsearch_score(unsigned char piece, unsigned char op_piece);

void movesort(MOVE moves[256], int sort[256], int length, int current);

void skipHashMove(MOVE moves[256], int sort[256], int length, MOVE *hash_move, unsigned char move_type, int exist);

int orderMove_root(MOVE moves[256], int sort[256], int length, MOVE *best_move, MOVE *hash_move);

#endif