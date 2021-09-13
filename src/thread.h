#pragma once

#include <stdbool.h>
#include <setjmp.h>
#include <pthread.h>
#include "board.h"
#include "move.h"
#include "transposition.h"

#define MAXDEPTH 100
#define MAINTHREAD 0

extern int search_depth;
extern pthread_mutex_t mutex;
extern pthread_cond_t cond;

typedef struct {

    BOARD pos; // start position when search
    int depth, color; // start depth and color
    char op_move[6]; // opponent move
    MOVE killers[MAXDEPTH][2];
    COUNTERMOVE counterMoves[12][64];
    int history[2][64][64];
    int nodes, tbhits;
    char BestMove[6];
    MOVE searched_move;
    unsigned long long history_log[800]; // log for repetition check
    struct Pawn Pawntt[PAWNHASHSIZE]; // pawn hash for each thread
    jmp_buf buf;

    int index; // thread index
    int count; // total threads

} THREAD;

extern THREAD *threads;

void createThreads(int count);

void freeThreads();

void prepareSearch(BOARD *pos, int piece_color, char op_move[6], unsigned long long *history_log);

void cleanupSearch();

int getTotalNodes();

int getTotalTbhits();

struct Pawn *probePawnTT(unsigned long long key, THREAD *thread);

void storePawnTT(unsigned long long key, short eval_mg, short eval_eg, THREAD *thread);

void wait_for_search();

void signal_uci_thread();