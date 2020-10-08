#ifndef ORDERMOVE_H
#define ORDERMOVE_H

#include "Move.h"

int wCapMove_score(MOVE *move, char piece, char op_piece, char board[8][8], int x, int y, int nx, int ny);

int bCapMove_score(MOVE *move, char piece, char op_piece, char board[8][8], int x, int y, int nx, int ny);

int promotion_score();

int underpromotion_score();

int castling_score();

int quietMove_score(MOVE *move, int x1, int y1, int x2, int y2, int ply, int color);

int wCapMove_score_root(MOVE *move, MOVE *best_move, MOVE *hash_move, char piece, char op_piece, char board[8][8], int x, int y, int nx, int ny);

int bCapMove_score_root(MOVE *move, MOVE *best_move, MOVE *hash_move, char piece, char op_piece, char board[8][8], int x, int y, int nx, int ny);

int promotion_score_root(MOVE *move, MOVE *best_move, MOVE *hash_move);

int underpromotion_score_root(MOVE *move, MOVE *best_move, MOVE *hash_move);

int castling_score_root(MOVE *move, MOVE *best_move, MOVE *hash_move);

int quietMove_score_root(MOVE *move, MOVE *best_move, MOVE *hash_move, int x1, int y1, int x2, int y2, int color);

int wCapQsearch_score(char piece, char op_piece, int new_x);

int bCapQsearch_score(char piece, char op_piece, int new_x);

int piece_value(char piece);

void movesort(MOVE moves[256], int sort[256], int length, int current);

int orderHashMove(MOVE moves[256], int sort[256], int length, MOVE *hash_move);

#endif