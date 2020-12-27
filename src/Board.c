#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "Board.h"
#include "Transposition.h"

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
const char chess_board[8][8] = {

            {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
            {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
            {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
            {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'},

};

// starting piece list
const int piece_list[12][10] = {

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

const int piece_count[12] = { 8, 2, 2, 2, 1, 1, 8, 2, 2, 2, 1, 1 };

const int index_board[64] = {

            0,   0,   0,   0,   0,   1,   1,   1,
            0,   1,   2,   3,   4,   5,   6,   7,
           -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
           -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
           -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
           -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            0,   1,   2,   3,   4,   5,   6,   7,
            0,   0,   0,   0,   0,   1,   1,   1

};

void displayboard(char board[8][8])
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
                printf("| %c |", board[i][k]);
            else
                printf("| %c ", board[i][k]);
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
    switch (position[1])
    {
        case '8':
            return 0;
        case '7':
            return 1;
        case '6':
            return 2;
        case '5':
            return 3;
        case '4':
            return 4;
        case '3':
            return 5;
        case '2':
            return 6;
        case '1':
            return 7;
    }

    return -1; // when nothing is matched
}

// get the y index from a position on the board
inline int position_to_y(char position[3])
{
    switch (position[0])
    {
        case 'a':
            return 0;
        case 'b':
            return 1;
        case 'c':
            return 2;
        case 'd':
            return 3;
        case 'e':
            return 4;
        case 'f':
            return 5;
        case 'g':
            return 6;
        case 'h':
            return 7;
    }

    return -1;
}

// convert 0 - 63 index to rank
inline int get_rank(int index)
{
    return index / 8;
}

// convert 0 - 63 index to file
inline int get_file(int index)
{
    return index % 8;
}

// initialize board struct to starting position
inline void init_board(BOARD *pos)
{
    memcpy(pos->board, chess_board, sizeof(pos->board));
    // set the castling flags
    pos->ksb = 1;
    pos->qsb = 1;
    pos->ksw = 1;
    pos->qsw = 1;
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

inline void clear_board(char board[8][8])
{
    const char board_reset[8][8] = {

                {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},

    };

    memcpy(board, board_reset, sizeof(board_reset));
}

// set piece lists based on the position
void set_piecelists(BOARD *pos)
{
    int i;
    pos->piece_count[wP] = pos->piece_count[wN] = pos->piece_count[wB] = 0;
    pos->piece_count[wR] = pos->piece_count[wQ] = 0;
    pos->piece_count[bP] = pos->piece_count[bN] = pos->piece_count[bB] = 0;
    pos->piece_count[bR] = pos->piece_count[bQ] = 0;

    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            i = 8 * x + y;
            switch (pos->board[x][y])
            {
                case 'P':
                    pos->piece_list[wP][pos->piece_count[wP]] = i;
                    pos->index_board[i] = pos->piece_count[wP];
                    pos->piece_count[wP]++;
                    break;
                case 'N':
                    pos->piece_list[wN][pos->piece_count[wN]] = i;
                    pos->index_board[i] = pos->piece_count[wN];
                    pos->piece_count[wN]++;
                    break;
                case 'B':
                    pos->piece_list[wB][pos->piece_count[wB]] = i;
                    pos->index_board[i] = pos->piece_count[wB];
                    pos->piece_count[wB]++;
                    break;
                case 'R':
                    pos->piece_list[wR][pos->piece_count[wR]] = i;
                    pos->index_board[i] = pos->piece_count[wR];
                    pos->piece_count[wR]++;
                    break;
                case 'Q':
                    pos->piece_list[wQ][pos->piece_count[wQ]] = i;
                    pos->index_board[i] = pos->piece_count[wQ];
                    pos->piece_count[wQ]++;
                    break;
                case 'K':
                    pos->piece_list[wK][0] = i;
                    pos->index_board[i] = 0;
                    break;
                case 'p':
                    pos->piece_list[bP][pos->piece_count[bP]] = i;
                    pos->index_board[i] = pos->piece_count[bP];
                    pos->piece_count[bP]++;
                    break;
                case 'n':
                    pos->piece_list[bN][pos->piece_count[bN]] = i;
                    pos->index_board[i] = pos->piece_count[bN];
                    pos->piece_count[bN]++;
                    break;
                case 'b':
                    pos->piece_list[bB][pos->piece_count[bB]] = i;
                    pos->index_board[i] = pos->piece_count[bB];
                    pos->piece_count[bB]++;
                    break;
                case 'r':
                    pos->piece_list[bR][pos->piece_count[bR]] = i;
                    pos->index_board[i] = pos->piece_count[bR];
                    pos->piece_count[bR]++;
                    break;
                case 'q':
                    pos->piece_list[bQ][pos->piece_count[bQ]] = i;
                    pos->index_board[i] = pos->piece_count[bQ];
                    pos->piece_count[bQ]++;
                    break;
                case 'k':
                    pos->piece_list[bK][0] = i;
                    pos->index_board[i] = 0;
                    break;
            }
        }
    }
}
