#ifndef SYZYGY_H
#define SYZYGY_H

#include "Board.h"
#include "Move.h"
#include "Fathom/tbprobe.h"

extern int TB_DEPTH;
extern uint64_t setMask[64];

void init_setMask();

unsigned tablebasesProbeWDL(BOARD *pos, int depth, int color);

int tablebasesProbeDTZ(BOARD *pos, MOVE *bestMove, int *score, int color);

#endif