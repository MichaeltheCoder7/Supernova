#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>
#include <assert.h>
#include "../src/move.h"
#include "../src/board.h"
#include "../src/movegen.h"
#include "../src/attack.h"
#include "../src/transposition.h"
#include "../src/checkmove.h"

// position 1
// depth 5: 4865609 nodes
// depth 6: 119060324 nodes
const unsigned char board1[8][8] = {

            { bR, bN, bB, bQ, bK, bB, bN, bR },
            { bP, bP, bP, bP, bP, bP, bP, bP },
            { __, __, __, __, __, __, __, __ },
            { __, __, __, __, __, __, __, __ },
            { __, __, __, __, __, __, __, __ },
            { __, __, __, __, __, __, __, __ },
            { wP, wP, wP, wP, wP, wP, wP, wP },
            { wR, wN, wB, wQ, wK, wB, wN, wR },

};

const unsigned char piece_list1[12][10] = {

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

const unsigned char piece_count1[12] = { 8, 2, 2, 2, 1, 1, 8, 2, 2, 2, 1, 1 };

const unsigned char index_board1[64] = {

             0,   0,   0,   0,   0,   1,   1,   1,
             0,   1,   2,   3,   4,   5,   6,   7,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
             0,   1,   2,   3,   4,   5,   6,   7,
             0,   0,   0,   0,   0,   1,   1,   1

};

// position 2
// depth 4: 4085603 nodes
// depth 5: 193690690 nodes
const unsigned char board2[8][8] = {

            { bR, __, __, __, bK, __, __, bR },
            { bP, __, bP, bP, bQ, bP, bB, __ },
            { bB, bN, __, __, bP, bN, bP, __ },
            { __, __, __, wP, wN, __, __, __ },
            { __, bP, __, __, wP, __, __, __ },
            { __, __, wN, __, __, wQ, __, bP },
            { wP, wP, wP, wB, wB, wP, wP, wP },
            { wR, __, __, __, wK, __, __, wR },

};

const unsigned char piece_list2[12][10] = {

            { a2, b2, c2, d5, e4, f2, g2, h2, -1, -1 }, // wP
            { c3, e5, -1, -1, -1, -1, -1, -1, -1, -1 }, // wN
            { d2, e2, -1, -1, -1, -1, -1, -1, -1, -1 }, // wB
            { a1, h1, -1, -1, -1, -1, -1, -1, -1, -1 }, // wR
            { f3, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // wQ
            { e1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // wK
            { a7, b4, c7, d7, e6, f7, g6, h3, -1, -1 }, // bP
            { b6, f6, -1, -1, -1, -1, -1, -1, -1, -1 }, // bN
            { a6, g7, -1, -1, -1, -1, -1, -1, -1, -1 }, // bB
            { a8, h8, -1, -1, -1, -1, -1, -1, -1, -1 }, // bR
            { e7, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // bQ
            { e8, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // bK

};

const unsigned char piece_count2[12] = { 8, 2, 2, 2, 1, 1, 8, 2, 2, 2, 1, 1 };

const unsigned char index_board2[64] = {

             0,  -1,  -1,  -1,   0,  -1,  -1,   1,
             0,  -1,   2,   3,   0,   5,   1,  -1,
             0,   0,  -1,  -1,   4,   1,   6,  -1,
            -1,  -1,  -1,   3,   1,  -1,  -1,  -1,
            -1,   1,  -1,  -1,   4,  -1,  -1,  -1,
            -1,  -1,   0,  -1,  -1,   0,  -1,   7,
             0,   1,   2,   0,   1,   5,   6,   7,
             0,  -1,  -1,  -1,   0,  -1,  -1,   1

};

static unsigned long long perft(BOARD *pos, int depth, int color)
{
    unsigned long long nodes = 0;
    int length;
    BOARD pos_copy;

    if (depth == 0)
        return 1ULL;

    // generate moves
    MOVE moves[256];
    int scores[256];
    length = moveGen(pos, moves, scores, 0, color);

    for (int x = 0; x < length; x++)
    {
        // make a copy of the board
        pos_copy = *pos;
        // make move
        makeMove(&pos_copy, &moves[x]);

        // skip if the king is left in check
        if (ifCheck(&pos_copy, color))
        {
            continue;
        }

        nodes += perft(&pos_copy, depth - 1, -color);
    }

    return nodes;
}

void init_board1(BOARD *pos)
{
    memcpy(pos->board, board1, sizeof(pos->board));
    // set the castling flags
    pos->ksb = 1;
    pos->qsb = 1;
    pos->ksw = 1;
    pos->qsw = 1;
    // set en passant files
    pos->ep_file = 0;
    pos->halfmove_counter = 0;
    pos->piece_num = 32;
    memcpy(pos->piece_list, piece_list1, sizeof(pos->piece_list));
    memcpy(pos->piece_count, piece_count1, sizeof(pos->piece_count));
    memcpy(pos->index_board, index_board1, sizeof(pos->index_board));
    pos->key = getHash(pos, -1);
    pos->pawn_key = getPawnHash(pos->board);
    pos->pawn_push = false;
}

void init_board2(BOARD *pos)
{
    memcpy(pos->board, board2, sizeof(pos->board));
    // set the castling flags
    pos->ksb = 1;
    pos->qsb = 1;
    pos->ksw = 1;
    pos->qsw = 1;
    // set en passant files
    pos->ep_file = 0;
    pos->halfmove_counter = 0;
    pos->piece_num = 32;
    memcpy(pos->piece_list, piece_list2, sizeof(pos->piece_list));
    memcpy(pos->piece_count, piece_count2, sizeof(pos->piece_count));
    memcpy(pos->index_board, index_board2, sizeof(pos->index_board));
    pos->key = getHash(pos, -1);
    pos->pawn_key = getPawnHash(pos->board);
    pos->pawn_push = false;
}

int main()
{
    BOARD pos;
    clock_t t;
    double time_taken = 0;
    init_zobrist();

    // define position and depth here
    int depth = 5;
    init_board2(&pos);

    printf("Testing move gen with perft...\n");
    printf("Depth: %d\n", depth);
    printf("Position:\n");
    displayboard(pos.board);

    t = clock();
    long long nodes = perft(&pos, depth, -1);
    t = clock() - t;
    time_taken += ((double)t) / CLOCKS_PER_SEC;

    printf("Node count: %lld\n", nodes);
    printf("Move gen time: %f sec\n", time_taken);

    return 0;
}