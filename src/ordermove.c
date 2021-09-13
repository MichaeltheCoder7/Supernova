#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>
#include "ordermove.h"
#include "see.h"
#include "search.h"
#include "move.h"
#include "transposition.h"

// swap two ints
static inline void swap(int *a, int *b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

// swap two move structs
static inline void swapMove(MOVE *move1, MOVE *move2)
{
    MOVE temp = *move1;
    *move1 = *move2;
    *move2 = temp;
}

// selection sort
inline void movesort(MOVE moves[256], int sort[256], int length, int current)
{
    // find the move with the highest score
    int high = current;

    for (int i = current + 1; i < length; i++)
    {
        if (sort[i] > sort[high])
            high = i;
    }

    swap(&sort[current], &sort[high]);
    swapMove(&moves[current], &moves[high]);
}

// find the hash move if it exists and put it at the beginning of the move list to skip it
inline void skipHashMove(MOVE moves[256], int sort[256], int length, MOVE *hash_move, unsigned char move_type, int exist)
{
    if (!exist)
    {
        return;
    }
    else
    {
        for (int x = 0; x < length; x++)
        {
            if (compareMove(&moves[x], hash_move))
            {
                sort[x] = HASHMOVE;
                swap(&sort[0], &sort[x]);
                swapMove(&moves[0], &moves[x]);
                moves[0].move_type = move_type;
                return;
            }
        }
    }
}

// assign scores and sort for the best move and hash move at root
// return number of moves sorted
inline int orderMove_root(MOVE moves[256], int sort[256], int length, MOVE *best_move, MOVE *hash_move)
{
    int move_count = 0;

    if (best_move->from != NOMOVE)
    {
        for (int x = 0; x < length; x++)
        {
            if (compareMove(&moves[x], best_move))
            {
                sort[x] = BESTMOVE;
                swap(&sort[0], &sort[x]);
                swapMove(&moves[0], &moves[x]);
                move_count++;
                break;
            }
        }
    }

    if (hash_move->from != NOMOVE && !compareMove(hash_move, best_move))
    {
        for (int x = 0; x < length; x++)
        {
            if (compareMove(&moves[x], hash_move))
            {
                sort[x] = HASHMOVE;
                if (move_count == 0)
                {
                    swap(&sort[0], &sort[x]);
                    swapMove(&moves[0], &moves[x]);
                }
                else
                {
                    swap(&sort[1], &sort[x]);
                    swapMove(&moves[1], &moves[x]);
                }
                move_count++;
                break;
            }
        }
    }

    return move_count;
}

// order captures with mvv-lva and SEE
inline int capMove_score(unsigned char piece, unsigned char op_piece, unsigned char board[8][8], int x1, int y1, int x2, int y2, int color)
{
    int mvv = mvv_score[op_piece];
    int lva = lva_score[piece];

    int target = piece_value[op_piece];
    if (target > piece_value[piece])
    {
        return mvv + lva + WCAPTURE;
    }

    int value = SEE_MO(board, x1, y1, x2, y2, target, color);

    if (value > 0)
        return mvv + lva + WCAPTURE;
    else if (value == 0)
        return mvv + lva + ECAPTURE;
    else
        return mvv + lva + LCAPTURE;
}

// return 1 if counter moves are the same, 0 otherwise
static inline int compareCounterMove(THREAD *thread, BOARD *pos, MOVE *move)
{
    if (pos->last_move.piece != NOMOVE
        && thread->counterMoves[pos->last_move.piece][pos->last_move.to].piece == pos->board[move->from / 8][move->from % 8]
        && thread->counterMoves[pos->last_move.piece][pos->last_move.to].to == move->to)
    {
        return 1;
    }

    return 0;
}

// quiet move ordering based on killer moves, counter move, and history heuristic
inline int quietMove_score(THREAD *thread, BOARD *pos, MOVE *move, int origin, int x, int y, int ply, int color)
{
    if (compareMove(&(thread->killers[ply][0]), move))
    {
        return KILLER1;
    }
    else if (compareMove(&(thread->killers[ply][1]), move))
    {
        return KILLER2;
    }
    else if (compareCounterMove(thread, pos, move))
    {
        return COUNTER;
    }
    else
    {
        return thread->history[(color == 1) ? 1 : 0][origin][8 * x + y];
    }
}

// capture ordering for qsearch
inline int capQsearch_score(unsigned char piece, unsigned char op_piece)
{
    int mvv = mvv_score[op_piece];
    int lva = lva_score[piece];

    // check if lower takes higher or equal
    if (piece_value[op_piece] >= piece_value[piece])
    {
        mvv += 600;
    }

    return mvv + lva;
}