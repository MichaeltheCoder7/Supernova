#ifndef ORDERMOVE_H
#define ORDERMOVE_H

#include "Move.h"

int wCapMove_score(char piece, char op_piece, char board[8][8], int x1, int y1, int x2, int y2);

int bCapMove_score(char piece, char op_piece, char board[8][8], int x1, int y1, int x2, int y2);

int promotion_score();

int underpromotion_score();

int castling_score();

int quietMove_score(MOVE *move, int origin, int x, int y, int ply, int color);

int wCapQsearch_score(char piece, char op_piece, int new_x);

int bCapQsearch_score(char piece, char op_piece, int new_x);

int piece_value(char piece);

void movesort(MOVE moves[256], int sort[256], int length, int current);

int orderHashMove(MOVE moves[256], int sort[256], int length, MOVE *hash_move);

void orderMove_root(MOVE moves[256], int sort[256], int length, MOVE *best_move, MOVE *hash_move);

#endif