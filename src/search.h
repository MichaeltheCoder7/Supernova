#ifndef SEARCH_H
#define SEARCH_H

#include <stdbool.h>
#include "board.h"
#include "move.h"
#include "thread.h"

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

extern int history_index;
extern bool stop;
extern double search_time;
extern bool ponderhit;
extern double ponder_time;
extern bool extra_time;
extern bool analyze;
extern bool time_management;
extern bool node_mode;
extern int search_nodes;
extern bool stop_search;

static const int futilityMargin[9] = { 0, 120, 220, 320, 420, 520, 620, 720, 820 };
static const int lmpMargin[9] = { 0, 5, 8, 12, 17, 23, 30, 38, 47 };

void init_lmr();

void clearKiller(THREAD *thread);

void ageHistory(THREAD *thread);

void clearCounterMoveTable(THREAD *thread);

MOVE internalID(THREAD *thread, BOARD *pos, int depth, int ply, int color, int alpha, int beta);

void search(BOARD *pos, int piece_color, char op_move[6], int thread_num, unsigned long long *history_log);

#endif