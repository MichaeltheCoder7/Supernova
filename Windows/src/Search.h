#ifndef SEARCH_H
#define SEARCH_H

#include <stdbool.h>
#include "Board.h"
#include "Move.h"

#define MAXDEPTH 40

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
bool node_mode;
int search_nodes;
bool stop_search;

void search(BOARD *pos, int piece_color, char op_move[6]);

#endif