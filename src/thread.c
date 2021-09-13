#include <stdlib.h>
#include <string.h>
#include "thread.h"
#include "search.h"
#include "board.h"

int search_depth;
THREAD *threads = NULL;

inline void createThreads(int count)
{
    if (threads)
        free(threads);

    // Allocate memory for thread struct
    threads = calloc(count, sizeof(THREAD));

    for (int i = 0; i < count; i++) {
        threads[i].count = count;
        threads[i].index = i;
    }
}

inline void freeThreads()
{
    free(threads);
    threads = NULL;
}

// clear and reset data between each search
inline void prepareSearch(BOARD *pos, int piece_color, char op_move[6], unsigned long long *history_log)
{
    for (int i = 0; i < threads->count; i++) {
        ageHistory(&threads[i]);
        clearKiller(&threads[i]); // clear killer move table
        memset(threads[i].BestMove, 0, sizeof(threads[i].BestMove));
        threads[i].nodes = 0, threads[i].tbhits = 0;
        threads[i].depth = (search_depth == -1) ? MAXDEPTH : search_depth;
        threads[i].color = piece_color;
        threads[i].pos = *pos;
        strncpy(threads[i].op_move, op_move, 5);
        threads[i].op_move[5] = '\0';
        memcpy(&threads[i].history_log, history_log, sizeof(threads[i].history_log));
    }
}

// pawn transposition table
inline struct Pawn *probePawnTT(unsigned long long key, THREAD *thread)
{
    int hashIndex = key % PAWNHASHSIZE;

    if (thread->Pawntt[hashIndex].valid == true && thread->Pawntt[hashIndex].key == key)
    {
        return &thread->Pawntt[hashIndex];
    }
    else
    {
        return NULL;
    }

}

inline void storePawnTT(unsigned long long key, short eval_mg, short eval_eg, THREAD *thread)
{
    int hashIndex = key % PAWNHASHSIZE;

    thread->Pawntt[hashIndex].key = key;
    thread->Pawntt[hashIndex].eval_mg = eval_mg;
    thread->Pawntt[hashIndex].eval_eg = eval_eg;
    thread->Pawntt[hashIndex].valid = true;
}

static inline void clearPawnTT(THREAD *thread)
{
    for (int x = 0; x < PAWNHASHSIZE; x++)
    {
        thread->Pawntt[x].key = 0;
        thread->Pawntt[x].eval_mg = 0;
        thread->Pawntt[x].eval_eg = 0;
        thread->Pawntt[x].valid = false;
    }
}

// clean countermove table, history table, and pawn transposition table
inline void cleanupSearch()
{   
    for (int i = 0; i < threads->count; i++) {
        clearCounterMoveTable(&threads[i]); // clear counter move table
        memset(threads[i].history, 0, sizeof(threads[i].history)); // clear history heuristic table
        clearPawnTT(&threads[i]);
    }
}

inline int getTotalNodes()
{
    int count = 0;

    for (int i = 0; i < threads->count; i++) {
        count += threads[i].nodes;
    }

    return count;
} 

inline int getTotalTbhits()
{
    int count = 0;

    for (int i = 0; i < threads->count; i++) {
        count += threads[i].tbhits;
    }

    return count;
} 