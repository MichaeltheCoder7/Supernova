#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>
#include "Board.h"
#include "Move.h"
#include "Transposition.h"
#include "Search.h"

#define EMPTY -1

int piece_code(char piece)
{ 
    switch(piece)
    {
        case 'P':
            return 0; 
        case 'N':
            return 1; 
        case 'B':
            return 2; 
        case 'R':
            return 3; 
        case 'Q': 
            return 4; 
        case 'K': 
            return 5; 
        case 'p': 
            return 6; 
        case 'n': 
            return 7; 
        case 'b': 
            return 8; 
        case 'r': 
            return 9; 
        case 'q': 
            return 10; 
        case 'k':
            return 11; 
        default:
            break;
    }
    
    return -1;
} 

//return a 64 bit random number
unsigned long long llrand() 
{
    unsigned long long r = 0;

    for (int i = 0; i < 5; ++i) {
        r = (r << 15) | (rand() & 0x7FFF);
    }

    return r & 0xFFFFFFFFFFFFFFFFULL;
}

//initialize zobrist hashing:
void init_zobrist()
{
    //fill a table of random numbers
    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            for(int k = 0; k < 12; k++)
            table[i][j][k] = llrand();
        }
    }
    turn = llrand();
    for(int i = 0; i < 8; i++)
    {
        ep[i] = llrand();
    }
    kswcr = llrand();
    qswcr = llrand();
    ksbcr = llrand();
    qsbcr = llrand();
}

//use zobrist hashing to encode chess board
unsigned long long getHash(BOARD *pos, int color)
{
    unsigned long long h = 0;
    int chess_piece;
    for(int x = 0; x < 8; x++)
    {
        for(int y = 0; y < 8; y++)
        {
            if(pos->board[x][y] != ' ')
            {
                chess_piece = piece_code(pos->board[x][y]);
                h ^= table[x][y][chess_piece];
            }
        }
    }
    //encode turn
    if(color == 1)
    {
        h ^= turn;
    }
    //encode castling rights
    if(pos->ksw)
    {
        h ^= kswcr;
    }
    if(pos->qsw)
    {
        h ^= qswcr;
    }
    if(pos->ksb)
    {
        h ^= ksbcr;
    }
    if(pos->qsb)
    {
        h ^= qsbcr;
    }
    //encode enpassant
    switch(pos->ep_file)
    {
        case 0:
            break;
        case 1:
            pos->key ^= ep[0];
            break;
        case 2:
            pos->key ^= ep[1];
            break;
        case 3:
            pos->key ^= ep[2];
            break;
        case 4:
            pos->key ^= ep[3];
            break;
        case 5:
            pos->key ^= ep[4];
            break;
        case 6:
            pos->key ^= ep[5];
            break;
        case 7:
            pos->key ^= ep[6];
            break;
        case 8:
            pos->key ^= ep[7];
            break;  
    }
    
    return h;
}

//transposition/hash table:
//+1 index for collision
struct DataItem *probeTT(unsigned long long key)
{
    //get the hash 
    int hashIndex = key % (HASHSIZE - 1);
    if(tt[hashIndex].flag != EMPTY && tt[hashIndex].key == key)
    {    
        tt[hashIndex].age = false;
        return &tt[hashIndex];
    }
    else if(tt[hashIndex + 1].flag != EMPTY && tt[hashIndex + 1].key == key)
    {    
        tt[hashIndex + 1].age = false;
        return &tt[hashIndex + 1];
    }
    else
    {
        return NULL;
    }

}

void storeTT(unsigned long long key, int evaluation, int statEval, int depth, MOVE *bestmove, int flag)
{
    if(stop_search) //don't save when time up
        return;

    //get the hash 
    int hashIndex = key % (HASHSIZE - 1);

    if(tt[hashIndex].flag == EMPTY || (tt[hashIndex].key == key && tt[hashIndex].depth <= depth) || tt[hashIndex].age == true)
    {
        tt[hashIndex].key = key;
        tt[hashIndex].evaluation = evaluation;
        tt[hashIndex].statEval = statEval;
        tt[hashIndex].flag = flag;
        tt[hashIndex].depth = depth;
        tt[hashIndex].bestmove = *bestmove;
        tt[hashIndex].age = false;
    }
    else if(tt[hashIndex].key != key && (tt[hashIndex + 1].depth <= depth || tt[hashIndex + 1].age == true))
    {
        tt[hashIndex + 1].key = key;
        tt[hashIndex + 1].evaluation = evaluation;
        tt[hashIndex + 1].statEval = statEval;
        tt[hashIndex + 1].flag = flag;
        tt[hashIndex + 1].depth = depth;
        tt[hashIndex + 1].bestmove = *bestmove;
        tt[hashIndex + 1].age = false;
    }
}

//set age in tt
//can print out percent of valid entries
void setAge(bool print_usage)
{
    float count = 0;
    for(int x = 0; x < HASHSIZE; x++)
    {
        if(tt[x].flag != EMPTY)
            count++;

        tt[x].age = true;
    }
    if(print_usage)
        printf("Table Usage: %.2f%%\n", count * 100 / HASHSIZE);
}

//clear the hash table
void clearTT()
{
    for(int x = 0; x < HASHSIZE; x++)
    {
        tt[x].key = 0;
        tt[x].depth = 0;
        tt[x].flag = EMPTY;
        tt[x].evaluation = 0;
        tt[x].statEval = VALUENONE;
        tt[x].age = false;
        tt[x].bestmove.from = NOMOVE;
        tt[x].bestmove.to = NOMOVE;
        tt[x].bestmove.promotion = ' ';
    }
}

struct Eval *probeEvalTT(unsigned long long key)
{
    //get the hash 
    int hashIndex = key % EVALHASHSIZE;
    if(Evaltt[hashIndex].valid == true && Evaltt[hashIndex].key == key)
    {    
        return &Evaltt[hashIndex];
    }
    else
    {
        return NULL;
    }

}

void storeEvalTT(unsigned long long key, int evaluation)
{
    //get the hash 
    int hashIndex = key % EVALHASHSIZE;

    Evaltt[hashIndex].key = key;
    Evaltt[hashIndex].evaluation = evaluation;
    Evaltt[hashIndex].valid = true;
}

void clearEvalTT()
{
    for(int x = 0; x < EVALHASHSIZE; x++)
    {
        Evaltt[x].key = 0;
        Evaltt[x].evaluation = 0;
        Evaltt[x].valid = false;
    }
}