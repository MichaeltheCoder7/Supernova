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

unsigned long long table[8][8][12];
unsigned long long turn;
unsigned long long kswcr;
unsigned long long qswcr;
unsigned long long ksbcr;
unsigned long long qsbcr;
unsigned long long ep[8];
long HASHSIZE;
long EVALHASHSIZE;
struct DataItem *tt;
struct Eval *Evaltt;
struct Pawn Pawntt[PAWNHASHSIZE];

inline int piece_code(char piece)
{
    switch (piece)
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

// return a 64 bit random number
inline unsigned long long llrand()
{
    unsigned long long r = 0;

    for (int i = 0; i < 5; i++)
    {
        r = (r << 15) | (rand() & 0x7FFF);
    }

    return r & 0xFFFFFFFFFFFFFFFFULL;
}

// initialize zobrist hashing:
inline void init_zobrist()
{
    // fill a table of random numbers
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            for (int k = 0; k < 12; k++)
                table[i][j][k] = llrand();
        }
    }

    for (int i = 0; i < 8; i++)
    {
        ep[i] = llrand();
    }

    turn = llrand();
    kswcr = llrand();
    qswcr = llrand();
    ksbcr = llrand();
    qsbcr = llrand();
}

// use zobrist hashing to encode chess board
inline unsigned long long getHash(BOARD *pos, int color)
{
    unsigned long long h = 0;
    int chess_piece;

    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            if (pos->board[x][y] != ' ')
            {
                chess_piece = piece_code(pos->board[x][y]);
                h ^= table[x][y][chess_piece];
            }
        }
    }

    // encode turn
    if (color == 1)
    {
        h ^= turn;
    }
    // encode castling rights
    if (pos->ksw)
    {
        h ^= kswcr;
    }
    if (pos->qsw)
    {
        h ^= qswcr;
    }
    if (pos->ksb)
    {
        h ^= ksbcr;
    }
    if (pos->qsb)
    {
        h ^= qsbcr;
    }

    // encode en passant
    switch (pos->ep_file)
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

inline int valueFromTT(int value, int ply)
{
    return (value > 19000) ? value - ply : (value < -19000) ? value + ply : value;
}

inline int valueToTT(int value, int ply)
{
    return (value > 19000) ? value + ply : (value < -19000) ? value - ply : value;
}

// transposition/hash table:
// +1 index for collision
inline struct DataItem *probeTT(unsigned long long key)
{
    // get the hash 
    int hashIndex = key % (HASHSIZE - 1);

    if (tt[hashIndex].flag != EMPTY && tt[hashIndex].key == key)
    {
        tt[hashIndex].age = false;
        return &tt[hashIndex];
    }
    else if (tt[hashIndex + 1].flag != EMPTY && tt[hashIndex + 1].key == key)
    {
        tt[hashIndex + 1].age = false;
        return &tt[hashIndex + 1];
    }
    else
    {
        return NULL;
    }
}

inline void storeTT(unsigned long long key, int evaluation, int statEval, int depth, MOVE *bestmove, int flag)
{
    if (stop_search) // don't save when time up
        return;

    // get the hash 
    int hashIndex = key % (HASHSIZE - 1);

    if (tt[hashIndex].flag == EMPTY || (tt[hashIndex].key == key && tt[hashIndex].depth <= depth) || tt[hashIndex].age == true)
    {
        tt[hashIndex].key = key;
        tt[hashIndex].evaluation = evaluation;
        tt[hashIndex].statEval = statEval;
        tt[hashIndex].flag = flag;
        tt[hashIndex].depth = depth;
        tt[hashIndex].bestmove = *bestmove;
        tt[hashIndex].age = false;
    }
    else if (tt[hashIndex].key != key && (tt[hashIndex + 1].depth <= depth || tt[hashIndex + 1].age == true))
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

// set age in tt
inline void setAge()
{
    for (int x = 0; x < HASHSIZE; x++)
    {
        tt[x].age = true;
    }
}

// clear the hash table
inline void clearTT()
{
    for (int x = 0; x < HASHSIZE; x++)
    {
        tt[x].key = 0;
        tt[x].depth = 0;
        tt[x].flag = EMPTY;
        tt[x].evaluation = 0;
        tt[x].statEval = VALUENONE;
        tt[x].age = false;
        clear_move(&tt[x].bestmove);
    }
}

inline struct Eval *probeEvalTT(unsigned long long key)
{
    int hashIndex = key % EVALHASHSIZE;

    if (Evaltt[hashIndex].valid == true && Evaltt[hashIndex].key == key)
    {
        return &Evaltt[hashIndex];
    }
    else
    {
        return NULL;
    }
}

inline void storeEvalTT(unsigned long long key, int evaluation)
{
    int hashIndex = key % EVALHASHSIZE;

    Evaltt[hashIndex].key = key;
    Evaltt[hashIndex].evaluation = evaluation;
    Evaltt[hashIndex].valid = true;
}

inline void clearEvalTT()
{
    for (int x = 0; x < EVALHASHSIZE; x++)
    {
        Evaltt[x].key = 0;
        Evaltt[x].evaluation = 0;
        Evaltt[x].valid = false;
    }
}

// include king positions
inline unsigned long long getPawnHash(char board[8][8])
{
    unsigned long long h = 0;

    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            switch (board[x][y])
            {
                case 'P':
                    h ^= table[x][y][wP];
                    break;
                case 'p':
                    h ^= table[x][y][bP];
                    break;
                case 'K':
                    h ^= table[x][y][wK];
                    break;
                case 'k':
                    h ^= table[x][y][bK];
                    break;
                default:
                    continue;
            }
        }
    }

    return h;
}

inline struct Pawn *probePawnTT(unsigned long long key)
{
    int hashIndex = key % PAWNHASHSIZE;

    if (Pawntt[hashIndex].valid == true && Pawntt[hashIndex].key == key)
    {
        return &Pawntt[hashIndex];
    }
    else
    {
        return NULL;
    }

}

inline void storePawnTT(unsigned long long key, short eval_mg, short eval_eg)
{
    int hashIndex = key % PAWNHASHSIZE;

    Pawntt[hashIndex].key = key;
    Pawntt[hashIndex].eval_mg = eval_mg;
    Pawntt[hashIndex].eval_eg = eval_eg;
    Pawntt[hashIndex].valid = true;
}

inline void clearPawnTT()
{
    for (int x = 0; x < PAWNHASHSIZE; x++)
    {
        Pawntt[x].key = 0;
        Pawntt[x].eval_mg = 0;
        Pawntt[x].eval_eg = 0;
        Pawntt[x].valid = false;
    }
}