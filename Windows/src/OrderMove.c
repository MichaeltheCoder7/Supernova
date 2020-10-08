#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>
#include "OrderMove.h"
#include "SEE.h"
#include "Search.h"
#include "Move.h"

#define BESTMOVE    1000000
#define HASHMOVE    900000
#define WCAPTURE    801200
#define PROMOTION   801200
#define CASTLING    801200
#define ECAPTURE    800600
#define KILLER1     800550
#define KILLER2     800540
#define UNDERPROM   800530
#define LCAPTURE    800000

//swap two ints
static inline void swap(int* a, int* b)
{ 
    int t = *a; 
    *a = *b; 
    *b = t; 
} 

//swap two move structs
static inline void swapMove(MOVE *move1, MOVE *move2) 
{ 
    MOVE temp = *move1;
    *move1 = *move2;
    *move2 = temp;
} 

//insertion sort
void movesort(MOVE moves[256], int sort[256], int length, int current) 
{
    //find the move with the highest score
    int high = current;

    for(int i = current+1; i < length ; i++)
    {
        if(sort[i] > sort[high])
            high = i;
    }

    swap(&sort[current], &sort[high]);
    swapMove(&moves[current], &moves[high]);
}

//return 1 if hash move exists
int orderHashMove(MOVE moves[256], int sort[256], int length, MOVE *hash_move)
{
    if(hash_move->from == NOMOVE)
    {
        return 0;
    }
    else
    {
        for(int x = 0; x < length; x++)
        {
            if(compareMove(&moves[x], hash_move))
            {
                sort[x] = HASHMOVE;
                swap(&sort[0], &sort[x]);
                swapMove(&moves[0], &moves[x]);
                return 1;
            }
        }
    }
    return 0;
}

int wCapMove_score(char piece, char op_piece, char board[8][8], int x1, int y1, int x2, int y2)
{
    int mvv = 0;
    int lva = 0;

    switch(piece)
    {
        case 'P':
            lva = 5;
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

    int target = piece_value(op_piece);
    if(target > piece_value(piece))
    {
        return mvv + lva + WCAPTURE;
    }

    int value = SEE_MO(board, x1, y1, x2, y2, target, -1);
    
    if(value > 0)
        return mvv + lva + WCAPTURE;
    else if(value == 0)
        return mvv + lva + ECAPTURE;
    else
        return mvv + lva + LCAPTURE;
}

int bCapMove_score(char piece, char op_piece, char board[8][8], int x1, int y1, int x2, int y2)
{
    int mvv = 0;
    int lva = 0;

    switch(piece)
    {
        case 'p':
            lva = 5;
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
    
    int target = piece_value(op_piece);
    if(target > piece_value(piece))
    {
        return mvv + lva + WCAPTURE;
    }

    int value = SEE_MO(board, x1, y1, x2, y2, target, 1);
    
    if(value > 0)
        return mvv + lva + WCAPTURE;
    else if(value == 0)
        return mvv + lva + ECAPTURE;
    else
        return mvv + lva + LCAPTURE;
}

int promotion_score()
{
    return PROMOTION;
}

int underpromotion_score()
{
    return UNDERPROM;
}

int castling_score()
{
    return CASTLING;
}

int quietMove_score(MOVE *move, int origin, int x, int y, int ply, int color)
{
    if(compareMove(&killers[ply][0], move))
    {
        return KILLER1;
    }
    else if(compareMove(&killers[ply][1], move))
    {
        return KILLER2;
    }
    else
    {
        return history[(color==1)?1:0][origin][8*x+y];
    }
}

int wCapMove_score_root(MOVE *move, MOVE *best_move, MOVE *hash_move, char piece, char op_piece, char board[8][8], int x1, int y1, int x2, int y2)
{
    int mvv = 0;
    int lva = 0;

    if(compareMove(best_move, move))
    {
        return BESTMOVE;
    }
    else if(compareMove(hash_move, move))
    {
        return HASHMOVE;
    }
    else
    {
        switch(piece)
        {
            case 'P':
                lva = 5;
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

        int target = piece_value(op_piece);
        if(target > piece_value(piece))
        {
            return mvv + lva + WCAPTURE;
        }

        int value = SEE_MO(board, x1, y1, x2, y2, target, -1);
        
        if(value > 0)
            return mvv + lva + WCAPTURE;
        else if(value == 0)
            return mvv + lva + ECAPTURE;
        else
            return mvv + lva + LCAPTURE;
    }
}

int bCapMove_score_root(MOVE *move, MOVE *best_move, MOVE *hash_move, char piece, char op_piece, char board[8][8], int x1, int y1, int x2, int y2)
{
    int mvv = 0;
    int lva = 0;

    if(compareMove(best_move, move))
    {
        return BESTMOVE;
    }
    else if(compareMove(hash_move, move))
    {
        return HASHMOVE;
    }
    else
    {
        switch(piece)
        {
            case 'p':
                lva = 5;
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

        int target = piece_value(op_piece);
        if(target > piece_value(piece))
        {
            return mvv + lva + WCAPTURE;
        }

        int value = SEE_MO(board, x1, y1, x2, y2, target, 1);
        
        if(value > 0)
            return mvv + lva + WCAPTURE;
        else if(value == 0)
            return mvv + lva + ECAPTURE;
        else
            return mvv + lva + LCAPTURE;
    }
}

int promotion_score_root(MOVE *move, MOVE *best_move, MOVE *hash_move)
{
    if(compareMove(best_move, move))
    {
        return BESTMOVE;
    }
    else if(compareMove(hash_move, move))
    {
        return HASHMOVE;
    }
    else
    {
        return PROMOTION;
    }
}

int underpromotion_score_root(MOVE *move, MOVE *best_move, MOVE *hash_move)
{
    if(compareMove(best_move, move))
    {
        return BESTMOVE;
    }
    else if(compareMove(hash_move, move))
    {
        return HASHMOVE;
    }
    else
    {
        return UNDERPROM;
    }
}

int castling_score_root(MOVE *move, MOVE *best_move, MOVE *hash_move)
{
    if(compareMove(best_move, move))
    {
        return BESTMOVE;
    }
    else if(compareMove(hash_move, move))
    {
        return HASHMOVE;
    }
    else
    {
        return CASTLING;
    }
}

int quietMove_score_root(MOVE *move, MOVE *best_move, MOVE *hash_move, int origin, int x, int y, int color)
{
    if(compareMove(best_move, move))
    {
        return BESTMOVE;
    }
    else if(compareMove(hash_move, move))
    {
        return HASHMOVE;
    }
    else if(compareMove(&killers[0][0], move))
    {
        return KILLER1;
    }
    else if(compareMove(&killers[0][1], move))
    {
        return KILLER2;
    }
    else
    {
        return history[(color==1)?1:0][origin][8*x+y];
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
        case ' ':
            return 100;
    }
    return 0;
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