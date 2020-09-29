#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>
#include "OrderMove.h"
#include "MoveGen.h"
#include "Search.h"
#include "Attack.h"

#define BESTMOVE    1000000
#define HASHMOVE    900000
#define CAPTURE     800000
#define PROMOTION   800500
#define UNDERPROM   800040
#define CASTLING    800050
#define KILLER1     800150
#define KILLER2     800140

//swap two ints
static inline void swap(int* a, int* b) 
{ 
    int t = *a; 
    *a = *b; 
    *b = t; 
} 

//swap two strings
static inline void swapstr(char *str1, char *str2) 
{ 
    char temp[6];
    strncpy(temp, str1, 6); 
    memmove(str1, str2, 6);
    strncpy(str2, temp, 6); 
} 

//insertion sort
void movesort(char moves[256][6], int sort[256], int length, int current) 
{
    //find the move with the highest score

    int high = current;

    for(int i = current+1; i < length ; i++) 
    {
        if(sort[i] > sort[high])
            high = i;
    }

    swap(&sort[current], &sort[high]);
    swapstr(moves[current], moves[high]);
}

int wCapMove_score(char move[6], char hash_move[6], char piece, char op_piece)
{
    int mvv = 0;
    int lva = 0;

    if(!strncmp(hash_move, move, 5))
    {
        return HASHMOVE;
    }
    else
    {
        switch(piece)
        {
            case 'P':
                lva = 5;
                //capture + promotion
                if(move[4] ==  'q')
                    lva += 500;
                break;
            case 'N':
                lva = 4;
                break;
            case 'B':
                lva = 3;
                break;
            case 'R':
                lva = 2;
                break;
            case 'Q':
                lva = 1;
                break;
            case 'K':
                lva = 0;
                break;
        }

        switch(op_piece)
        {
            case 'p':
                mvv = 100;
                break;
            case 'n':
                mvv = 200;
                break;
            case 'b':
                mvv = 300;
                break;
            case 'r':
                mvv = 400;
                break;
            case 'q':
                mvv = 500;
                break;
            case ' ':
                mvv = 100;
                break;
        }
        return mvv + lva + CAPTURE;
    }
}

int bCapMove_score(char move[6], char hash_move[6], char piece, char op_piece)
{
    int mvv = 0;
    int lva = 0;

    if(!strncmp(hash_move, move, 5))
    {
        return HASHMOVE;
    }
    else
    {
        switch(piece)
        {
            case 'p':
                lva = 5;
                //capture + promotion
                if(move[4] ==  'q')
                    lva += 500;
                break;
            case 'n':
                lva = 4;
                break;
            case 'b':
                lva = 3;
                break;
            case 'r':
                lva = 2;
                break;
            case 'q':
                lva = 1;
                break;
            case 'k':
                lva = 0;
                break;
        }

        switch(op_piece)
        {
            case 'P':
                mvv = 100;
                break;
            case 'N':
                mvv = 200;
                break;
            case 'B':
                mvv = 300;
                break;
            case 'R':
                mvv = 400;
                break;
            case 'Q':
                mvv = 500;
                break;
            case ' ':
                mvv = 100;
                break;
        }
        return mvv + lva + CAPTURE;
    }
}

int promotion_score(char move[6], char hash_move[6])
{
    if(!strncmp(hash_move, move, 5))
    {
        return HASHMOVE;
    }
    else
    {
        return PROMOTION;
    }
}

int underpromotion_score(char move[6], char hash_move[6])
{
    if(!strncmp(hash_move, move, 5))
    {
        return HASHMOVE;
    }
    else
    {
        return UNDERPROM;
    }
}

int castling_score(char move[6], char hash_move[6])
{
    if(!strncmp(hash_move, move, 5))
    {
        return HASHMOVE;
    }
    else
    {
        return CASTLING;
    }
}

int quietMove_score(char move[6], char hash_move[6], int x1, int y1, int x2, int y2, int ply, int color)
{
    if(!strncmp(hash_move, move, 5))
    {
        return HASHMOVE;
    }
    else if(!strncmp(killers[ply][0], move, 5))
    {
        return KILLER1;
    }
    else if(!strncmp(killers[ply][1], move, 5))
    {
        return KILLER2;
    }
    else
    {
        return history[(color==1)?1:0][8*x1 + x2][8*y1 + y2];
    }
}

int wCapMove_score_root(char move[6], char best_move[6], char hash_move[6], char piece, char op_piece)
{
    int mvv = 0;
    int lva = 0;

    if(!strncmp(best_move, move, 5))
    {
        return BESTMOVE;
    }
    else if(!strncmp(hash_move, move, 5))
    {
        return HASHMOVE;
    }
    else
    {
        switch(piece)
        {
            case 'P':
                lva = 5;
                //capture + promotion
                if(move[4] ==  'q')
                    lva += 500;
                break;
            case 'N':
                lva = 4;
                break;
            case 'B':
                lva = 3;
                break;
            case 'R':
                lva = 2;
                break;
            case 'Q':
                lva = 1;
                break;
            case 'K':
                lva = 0;
                break;
        }

        switch(op_piece)
        {
            case 'p':
                mvv = 100;
                break;
            case 'n':
                mvv = 200;
                break;
            case 'b':
                mvv = 300;
                break;
            case 'r':
                mvv = 400;
                break;
            case 'q':
                mvv = 500;
                break;
            case ' ':
                mvv = 100;
                break;
        }
        return mvv + lva + CAPTURE;
    }
}

int bCapMove_score_root(char move[6], char best_move[6], char hash_move[6], char piece, char op_piece)
{
    int mvv = 0;
    int lva = 0;

    if(!strncmp(best_move, move, 5))
    {
        return BESTMOVE;
    }
    else if(!strncmp(hash_move, move, 5))
    {
        return HASHMOVE;
    }
    else
    {
        switch(piece)
        {
            case 'p':
                lva = 5;
                //capture + promotion
                if(move[4] ==  'q')
                    lva += 500;
                break;
            case 'n':
                lva = 4;
                break;
            case 'b':
                lva = 3;
                break;
            case 'r':
                lva = 2;
                break;
            case 'q':
                lva = 1;
                break;
            case 'k':
                lva = 0;
                break;
        }

        switch(op_piece)
        {
            case 'P':
                mvv = 100;
                break;
            case 'N':
                mvv = 200;
                break;
            case 'B':
                mvv = 300;
                break;
            case 'R':
                mvv = 400;
                break;
            case 'Q':
                mvv = 500;
                break;
            case ' ':
                mvv = 100;
                break;
        }
        return mvv + lva + CAPTURE;
    }
}

int promotion_score_root(char move[6], char best_move[6], char hash_move[6])
{
    if(!strncmp(best_move, move, 5))
    {
        return BESTMOVE;
    }
    else if(!strncmp(hash_move, move, 5))
    {
        return HASHMOVE;
    }
    else
    {
        return PROMOTION;
    }
}

int underpromotion_score_root(char move[6], char best_move[6], char hash_move[6])
{
    if(!strncmp(best_move, move, 5))
    {
        return BESTMOVE;
    }
    else if(!strncmp(hash_move, move, 5))
    {
        return HASHMOVE;
    }
    else
    {
        return UNDERPROM;
    }
}

int castling_score_root(char move[6], char best_move[6], char hash_move[6])
{
    if(!strncmp(best_move, move, 5))
    {
        return BESTMOVE;
    }
    else if(!strncmp(hash_move, move, 5))
    {
        return HASHMOVE;
    }
    else
    {
        return CASTLING;
    }
}

int quietMove_score_root(char move[6], char best_move[6], char hash_move[6], int x1, int y1, int x2, int y2, int color)
{
    if(!strncmp(best_move, move, 5))
    {
        return BESTMOVE;
    }
    else if(!strncmp(hash_move, move, 5))
    {
        return HASHMOVE;
    }
    else if(!strncmp(killers[0][0], move, 5))
    {
        return KILLER1;
    }
    else if(!strncmp(killers[0][1], move, 5))
    {
        return KILLER2;
    }
    else
    {
        return history[(color==1)?1:0][8*x1 + x2][8*y1 + y2];
    }
}

int piece_value(char piece)
{
    switch(piece)
    {
        case 'P':
            return 100;
        case 'p':
            return 100;
        case 'N':
            return 320;
        case 'n':
            return 320;
        case 'B':
            return 330;
        case 'b':
            return 330;
        case 'R':
            return 500;
        case 'r':
            return 500;
        case 'Q':
            return 900;
        case 'q':
            return 900;
        default:
            return 100;
    }
}

int wCapQsearch_score(char piece, char op_piece, int new_x)
{
    int mvv = 0;
    int lva = 0;

    switch(piece)
    {
        case 'P':
            lva = 5;
            //capture + promotion
            if(new_x == 0)
                lva += 500;
            break;
        case 'N':
            lva = 4;
            break;
        case 'B':
            lva = 3;
            break;
        case 'R':
            lva = 2;
            break;
        case 'Q':
            lva = 1;
            break;
        case 'K':
            lva = 0;
            break;
    }

    switch(op_piece)
    {
        case 'p':
            mvv = 100;
            break;
        case 'n':
            mvv = 200;
            break;
        case 'b':
            mvv = 300;
            break;
        case 'r':
            mvv = 400;
            break;
        case 'q':
            mvv = 500;
            break;
        case ' ':
            mvv = 100;
            break;
    }

    //check if lower takes higher or equal
    if(piece_value(op_piece) >= piece_value(piece))
    {
        mvv += 600;
    }

    return mvv + lva;
}

int bCapQsearch_score(char piece, char op_piece, int new_x)
{
    int mvv = 0;
    int lva = 0;

    switch(piece)
    {
        case 'p':
            lva = 5;
            //capture + promotion
            if(new_x == 7)
                lva += 500;
            break;
        case 'n':
            lva = 4;
            break;
        case 'b':
            lva = 3;
            break;
        case 'r':
            lva = 2;
            break;
        case 'q':
            lva = 1;
            break;
        case 'k':
            lva = 0;
            break;
    }

    switch(op_piece)
    {
        case 'P':
            mvv = 100;
            break;
        case 'N':
            mvv = 200;
            break;
        case 'B':
            mvv = 300;
            break;
        case 'R':
            mvv = 400;
            break;
        case 'Q':
            mvv = 500;
            break;
        case ' ':
            mvv = 100;
            break;
    }

    //check if lower takes higher or equal
    if(piece_value(op_piece) >= piece_value(piece))
    {
        mvv += 600;
    }

    return mvv + lva;
}