#ifndef MOVE_H
#define MOVE_H

#include "board.h"

#define NOMOVE 100

typedef struct {

    unsigned char from;
    unsigned char to;
    char promotion; // ' ': not promotion 'q': queen 'r': rook 'b': bishop 'n': knight 
    unsigned char move_type; // 0: quiet 1: capture 2: promotion 3: castling

} MOVE;

static const char promotions[5] = { ' ', 'q', 'r', 'b', 'n' };

void print_move(MOVE move);

void clear_move(MOVE *move);

int compareMove(MOVE *move1, MOVE *move2);

MOVE string_to_move(char move[6]);

void move_to_string(MOVE *smove, char move[6]);

int makeMove(BOARD *pos, MOVE *move);

int makeMove_qsearch(BOARD *pos, MOVE *move);

void make_nullmove(BOARD *pos);

void makeMove_SEE(unsigned char board[8][8], int cur_x, int cur_y, int new_x, int new_y);

#endif