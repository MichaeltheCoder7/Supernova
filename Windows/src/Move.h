#ifndef MOVE_H
#define MOVE_H

#include "Board.h"

#define NOMOVE 100

typedef struct {

    unsigned char from;
    unsigned char to;
    char promotion;

} MOVE;

void print_move(MOVE move);

void clear_move(MOVE *move);

int compareMove(MOVE *move1, MOVE *move2);

MOVE string_to_move(char move[6]);

void move_to_string(MOVE *smove, char move[6]);

int makeMove(BOARD *pos, MOVE *move);

int makeMove_qsearch(BOARD *pos, MOVE *move);

int make_nullmove(BOARD *pos);

void undo_nullmove(BOARD *pos, int ep_file);

void makeMove_SEE(char board[8][8], int cur_x, int cur_y, int new_x, int new_y);

#endif