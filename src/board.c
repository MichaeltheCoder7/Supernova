#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>
#include "board.h"
#include "transposition.h"

/*
black pieces
00 01 02 03 04 05 06 07
10 11 12 13 14 15 16 17
20 21 22 23 24 25 26 27
30 31 32 33 34 35 36 37
40 41 42 43 44 45 46 47
50 51 52 53 54 55 56 57
60 61 62 63 64 65 66 67
70 71 72 73 74 75 76 77
white pieces
*/

/*
black pieces
 0  1  2  3  4  5  6  7
 8  9 10 11 12 13 14 15
16 17 18 19 20 21 22 23
24 25 26 27 28 29 30 31
32 33 34 35 36 37 38 39
40 41 42 43 44 45 46 47
48 49 50 51 52 53 54 55
56 57 58 59 60 61 62 63
white pieces
*/

// starting board
const unsigned char chess_board[8][8] = {

            { bR, bN, bB, bQ, bK, bB, bN, bR },
            { bP, bP, bP, bP, bP, bP, bP, bP },
            { __, __, __, __, __, __, __, __ },
            { __, __, __, __, __, __, __, __ },
            { __, __, __, __, __, __, __, __ },
            { __, __, __, __, __, __, __, __ },
            { wP, wP, wP, wP, wP, wP, wP, wP },
            { wR, wN, wB, wQ, wK, wB, wN, wR },

};

// starting piece list
// -1 is 255 in unsigned char
// it means no square
const unsigned char piece_list[12][10] = {

            { a2, b2, c2, d2, e2, f2, g2, h2, -1, -1 }, // wP
            { b1, g1, -1, -1, -1, -1, -1, -1, -1, -1 }, // wN
            { c1, f1, -1, -1, -1, -1, -1, -1, -1, -1 }, // wB
            { a1, h1, -1, -1, -1, -1, -1, -1, -1, -1 }, // wR
            { d1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // wQ
            { e1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // wK
            { a7, b7, c7, d7, e7, f7, g7, h7, -1, -1 }, // bP
            { b8, g8, -1, -1, -1, -1, -1, -1, -1, -1 }, // bN
            { c8, f8, -1, -1, -1, -1, -1, -1, -1, -1 }, // bB
            { a8, h8, -1, -1, -1, -1, -1, -1, -1, -1 }, // bR
            { d8, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // bQ
            { e8, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // bK

};

const unsigned char piece_count[12] = { 8, 2, 2, 2, 1, 1, 8, 2, 2, 2, 1, 1 };

const unsigned char index_board[64] = {

            0,   0,   0,   0,   0,   1,   1,   1,
            0,   1,   2,   3,   4,   5,   6,   7,
           -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
           -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
           -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
           -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            0,   1,   2,   3,   4,   5,   6,   7,
            0,   0,   0,   0,   0,   1,   1,   1

};

void displayboard(unsigned char board[8][8])
{
    int j = 8; // row numbers

    printf("\n    a   b   c   d   e   f   g   h\n");
    // print the board
    for (int i = 0; i < 8; i++)
    {
        printf("  +---+---+---+---+---+---+---+---+\n");
        printf(" %d", j);
        for (int k = 0; k < 8; k++)
        {
            if (k == 7)
                printf("| %c |", pieceTypes[board[i][k]]);
            else
                printf("| %c ", pieceTypes[board[i][k]]);
        }
        printf("%d", j);
        printf("\n");
        j--;
    }
    printf("  +---+---+---+---+---+---+---+---+\n");
    printf("    a   b   c   d   e   f   g   h\n\n");
}

// get the x index from a position on the board
inline int position_to_x(char position[3])
{
    return 56 - ((int) position[1]);
}

// get the y index from a position on the board
inline int position_to_y(char position[3])
{
    return ((int) position[0]) - 97;
}

// initialize board struct to starting position
inline void init_board(BOARD *pos)
{
    memcpy(pos->board, chess_board, sizeof(pos->board));
    // set the castling flag to 1111
    pos->castle_flag = 15;
    // set en passant files
    pos->ep_file = 0;
    pos->halfmove_counter = 0;
    pos->piece_num = 32;
    memcpy(pos->piece_list, piece_list, sizeof(pos->piece_list));
    memcpy(pos->piece_count, piece_count, sizeof(pos->piece_count));
    memcpy(pos->index_board, index_board, sizeof(pos->index_board));
    pos->key = getHash(pos, -1);
    pos->pawn_key = getPawnHash(pos->board);
    pos->pawn_push = false;
    pos->last_move.piece = NOMOVE;
    pos->last_move.to = NOMOVE;
}

inline void clear_board(unsigned char board[8][8])
{
    const unsigned char board_reset[8][8] = {

                { __, __, __, __, __, __, __, __ },
                { __, __, __, __, __, __, __, __ },
                { __, __, __, __, __, __, __, __ },
                { __, __, __, __, __, __, __, __ },
                { __, __, __, __, __, __, __, __ },
                { __, __, __, __, __, __, __, __ },
                { __, __, __, __, __, __, __, __ },
                { __, __, __, __, __, __, __, __ },

    };

    memcpy(board, board_reset, sizeof(board_reset));
}

// set piece lists based on the position
void set_piecelists(BOARD *pos)
{
    int i;

    // clear piece count array
    for (int piece = wP; piece <= bK; piece++)
    {
        pos->piece_count[piece] = 0;
    }

    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            i = 8 * x + y;
            if (pos->board[x][y] != __)
            {
                pos->piece_list[pos->board[x][y]][pos->piece_count[pos->board[x][y]]] = i;
                pos->index_board[i] = pos->piece_count[pos->board[x][y]];
                pos->piece_count[pos->board[x][y]]++;
            }
        }
    }
}
