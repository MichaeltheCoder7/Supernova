#ifndef SEARCH_H
#define SEARCH_H

#include <stdbool.h>
#include "Board.h"
#include "Move.h"

#define MAXDEPTH 100

unsigned long long history_log[800];
int history_index;
bool stop;
char pv_table[MAXDEPTH][6];
MOVE killers[MAXDEPTH][2];
double search_time;
int history[2][64][64];
bool ponderhit;
double ponder_time;
bool extra_time;
bool analyze;
int search_depth;
bool time_management;
bool node_mode;
int search_nodes;
bool stop_search;

static const int futilityMargin[9] = { 0, 120, 220, 320, 420, 520, 620, 720, 820 };
static const int lmpMargin[9] = { 0, 5, 8, 12, 17, 23, 30, 38, 47 };

MOVE internalID(BOARD *pos, int depth, int ply, int color, int alpha, int beta);

void search(BOARD *pos, int piece_color, char op_move[6]);

#endif