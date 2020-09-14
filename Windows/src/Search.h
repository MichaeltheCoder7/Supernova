#ifndef SEARCH_H
#define SEARCH_H

#include <stdbool.h>

#define MAXDEPTH 30

unsigned long long int table[8][8][12];
unsigned long long int turn;
unsigned long long int kswcr; 
unsigned long long int qswcr;
unsigned long long int ksbcr;
unsigned long long int qsbcr;
unsigned long long int ep[8]; 
unsigned long long int history_log[600];
int history_index;
bool stop;
char pv_table[MAXDEPTH][6];
char killers[MAXDEPTH][2][6];
double search_time;
int history[2][64][64];
bool ponderhit;
double ponder_time;
bool extra_time;
bool analyze;
unsigned long int HASHSIZE;
int search_depth;

struct DataItem 
{
   unsigned long long int key;   
   int evaluation;
   char depth;
   char flag;
   char bestmove[6];
   bool age;
};

struct DataItem* tt;  

void search(char board[8][8], int piece_color, char op_cp[3], char op_np[3], int ksw, int qsw, int ksb, int qsb, int move_counter);

int index(char piece);
 
void init_zobrist();

unsigned long long int getHash(char board[8][8], int color, char op_cp[3], char op_np[3], int ksw, int qsw, int ksb, int qsb);

void clearTT();


#endif
