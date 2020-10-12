#ifndef TRANSPOSITION_H
#define TRANSPOSITION_H

#include <stdbool.h>
#include "Board.h"
#include "Move.h"

#define PAWNHASHSIZE 667013 //10MB
#define VALUENONE -20100

unsigned long long table[8][8][12];
unsigned long long turn;
unsigned long long kswcr; 
unsigned long long qswcr;
unsigned long long ksbcr;
unsigned long long qsbcr;
unsigned long long ep[8]; 
unsigned long HASHSIZE;
unsigned long EVALHASHSIZE;

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

struct DataItem* tt;

struct Eval* Evaltt;

struct Pawn Pawntt[PAWNHASHSIZE];

int piece_code(char piece);

unsigned long long llrand();
 
void init_zobrist();

unsigned long long getHash(BOARD *pos, int color);

struct DataItem *probeTT(unsigned long long key);

void storeTT(unsigned long long key, int evaluation, int statEval, int depth, MOVE *bestmove, int flag);

void clearTT();

void setAge(bool print_usage);

struct Eval *probeEvalTT(unsigned long long key);

void storeEvalTT(unsigned long long key, int evaluation);

void clearEvalTT();

unsigned long long getPawnHash(char board[8][8]);

struct Pawn *probePawnTT(unsigned long long key);

void storePawnTT(unsigned long long key, short eval_mg, short eval_eg);

void clearPawnTT();

#endif