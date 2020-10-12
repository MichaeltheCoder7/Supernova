#ifndef BOARD__H
#define BOARD__H

#include <stdbool.h>

enum{ wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK };

enum positions {
    
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1
};

typedef struct{

    unsigned long long key;
    unsigned long long pawn_key;
    char board[8][8];
    int ksw;
    int qsw;
    int ksb;
    int qsb;
    int ep_file;
    int piece_num;
    int halfmove_counter;
    int piece_list[12][10];
    int piece_count[12];
    int index_board[64];
    bool pawn_push;

} BOARD;

void displayboard(char board[8][8]);

int position_to_x(char position[3]);

int position_to_y(char position[3]);

void init_board(BOARD *pos);

void clear_board(char board[8][8]);

void set_piecelists(BOARD *pos);

#endif