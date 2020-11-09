#ifndef TRANSPOSITION_H
#define TRANSPOSITION_H

#include <stdbool.h>
#include "Board.h"
#include "Move.h"

#define PAWNHASHSIZE 667013 //~10MB
#define VALUENONE -20100

extern unsigned long long table[8][8][12];
extern unsigned long long turn;
extern unsigned long long kswcr;
extern unsigned long long qswcr;
extern unsigned long long ksbcr;
extern unsigned long long qsbcr;
extern unsigned long long ep[8];
extern long HASHSIZE;
extern long EVALHASHSIZE;

struct DataItem 
{
   unsigned long long key;
   int evaluation;
   short statEval;
   char depth;
   MOVE bestmove;
   char flag;
   bool age;
};

struct Eval 
{
   unsigned long long key;   
   int evaluation;
   bool valid;
};

struct Pawn
{
   unsigned long long key;
   short eval_mg;
   short eval_eg;
   bool valid;
};

extern struct DataItem* tt;
extern struct Eval* Evaltt;
extern struct Pawn Pawntt[PAWNHASHSIZE];

int piece_code(char piece);

unsigned long long llrand();
 
void init_zobrist();

unsigned long long getHash(BOARD *pos, int color);

int valueFromTT(int value, int ply);

int valueToTT(int value, int ply);

struct DataItem *probeTT(unsigned long long key);

void storeTT(unsigned long long key, int evaluation, int statEval, int depth, MOVE *bestmove, int flag);

void clearTT();

void setAge();

struct Eval *probeEvalTT(unsigned long long key);

void storeEvalTT(unsigned long long key, int evaluation);

void clearEvalTT();

unsigned long long getPawnHash(char board[8][8]);

struct Pawn *probePawnTT(unsigned long long key);

void storePawnTT(unsigned long long key, short eval_mg, short eval_eg);

void clearPawnTT();

#endif