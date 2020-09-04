#ifndef EVALUATE_H
#define EVALUATE_H

#include <stdbool.h>

unsigned long int EVALHASHSIZE;

struct Eval 
{
   unsigned long long int key;   
   int evaluation;
   bool valid;
};

struct Eval* Evaltt;

void clearEvalTT();

void EvalTT_usage();

int evaluate(char board[8][8], int color);

#endif