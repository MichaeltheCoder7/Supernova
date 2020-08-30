#ifndef EVALUATE_H
#define EVALUATE_H

#include <stdbool.h>

#define EVALSIZE 1600421 //25MB

struct Eval 
{
   unsigned long long int key;   
   int evaluation;
   bool valid;
};

struct Eval Evaltt[EVALSIZE]; 

void clearEvalTT(bool print_usage);

int evaluate(char board[8][8], int color);

#endif