#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>
#include "Board.h"
#include "Evaluate.h"
#include "Transposition.h"

#define CONNECTEDPAWN   10
#define DOUBLEDPAWN     20
#define ISOLATEDPAWN    20
#define BACKWARDPAWN    8
#define ROOKOPENFILE    16
#define ROOKHALFFILE    8
#define ROOKQUEENFILE   7
#define BISHOPPAIR      30
#define BADBISHOP       6
#define OUTPOST         10
#define KNIGHTPAIR      8
#define ROOKPAIR        16
#define RETURNINGBISHOP 30 
#define TEMPO           10 

int PawnPassed_black[8] = { 0, 15, 20, 32, 56, 92, 140, 0 }; 
int PawnPassed_white[8] = { 0, 140, 92, 56, 32, 20, 15, 0 }; 
int knight_val[9] = { -20, -16, -12, -8, -4,  0,  4,  8, 10 };
int rook_val[9] = { 15, 12,  9,  6,  3,  0, -3, -6, -9 };
int wattack_count;
int battack_count;
int wattack_weight;
int battack_weight;

const int SafetyTable[100] = {

                    0,   0,   1,   2,   3,   4,   6,   8,  10,  12,
                    15,  18,  21,  24,  28,  32,  36,  40,  45,  50,
                    55,  60,  66,  72,  78,  84,  91,  98, 105, 112,
                    120, 128, 136, 144, 153, 162, 171, 180, 190, 200,
                    210, 220, 231, 242, 253, 264, 276, 288, 300, 312,
                    324, 336, 348, 360, 372, 384, 395, 400, 400, 400,
                    400, 400, 400, 400, 400, 400, 400, 400, 400, 400,
                    400, 400, 400, 400, 400, 400, 400, 400, 400, 400,
                    400, 400, 400, 400, 400, 400, 400, 400, 400, 400,
                    400, 400, 400, 400, 400, 400, 400, 400, 400, 400
                    
                    };

//piece-square tables:
//add bonus points based on position
int white_pawn[8][8] = {

                    {  0,   0,	 0,   0,   0,   0,   0,	  0 },
                    { 20,  20,	20,  30,  30,  20,  20,	 20 },
                    { 10,  10,	10,  20,  20,  10,  10,	 10 },
                    {  5,   5,	 5,  10,  10,	5,   5,	  5 },
                    {  0,   0,	10,  20,  20,  10,   0,	  0 },
                    {  5,   0,	 0,   5,   5,	0,   0,	  5 },
                    { 10,  10,	 5, -15, -15,   5,  10,	 10 },
                    {  0,   0,	 0,   0,   0,	0,   0,	  0 },	

                    };


int white_knight[8][8] = {

                    { -10, -10, -10, -10, -10, -10, -10, -10 },
                    { -10,   0,   0,   0,   0,   0,   0, -10 },
                    { -10,   0,   5,   5,   5,   5,   0, -10 },                  
                    { -10,   0,   5,  10,  10,   5,   0, -10 },
                    { -10,   0,   5,  10,  10,   5,   0, -10 },
                    { -10,   0,   5,   5,   5,   5,   0, -10 },
                    { -10,   0,   0,   0,   0,   0,   0, -10 },	
                    { -10, -30, -10, -10, -10, -10, -30, -10 },

                    };


int white_bishop[8][8] = {

                    { -10, -10, -10, -10, -10, -10, -10, -10 },
                    { -10,   0,   0,   0,   0,   0,   0, -10 },
                    { -10,   0,   5,   5,   5,   5,   0, -10 },                  
                    { -10,   0,   5,  10,  10,   5,   0, -10 },
                    { -10,   0,   5,  10,  10,   5,   0, -10 },
                    { -10,   0,   5,   5,   5,   5,   0, -10 },
                    { -10,   0,   0,   0,   0,   0,   0, -10 },	
                    { -10, -10, -20, -10, -10, -20, -10, -10 },

                    };

int white_rook[8][8] = {

                    {  5,   5,   5,   5,   5,   5,   5,   5 },
                    { 20,  20,  20,  20,  20,  20,  20,  20 },
                    { -5,   0,   0,   0,   0,   0,   0,  -5 },                    
                    { -5,   0,   0,   0,   0,   0,   0,  -5 },
                    { -5,   0,   0,   0,   0,   0,   0,  -5 },
                    { -5,   0,   0,   0,   0,   0,   0,  -5 },
                    { -5,   0,   0,   0,   0,   0,   0,  -5 },
                    {  0,   0,   0,   2,   2,   0,   0,   0 },	

                    };

int white_queen[8][8] = {

                    { -20, -10, -10,  -5,  -5, -10, -10, -20 },
                    { -10,   0,   0,   0,   0,   0,   0, -10 },
                    { -10,   0,   5,   5,   5,   5,   0, -10 },
                    {  -5,   0,   5,   5,   5,   5,   0,  -5 },
                    {   0,   0,   5,   5,   5,   5,   0,  -5 },
                    { -10,   5,   5,   5,   5,   5,   0, -10 },
                    { -10,   0,   5,   0,   0,   0,   0, -10 },
                    { -20, -10, -10,  -5,  -5, -10, -10, -20 },

                    };

int white_king_midgame[8][8] = {

                    { -30, -40, -40, -50, -50, -40, -40, -30 },
                    { -30, -40, -40, -50, -50, -40, -40, -30 },
                    { -30, -40, -40, -50, -50, -40, -40, -30 },
                    { -30, -40, -40, -50, -50, -40, -40, -30 },
                    { -20, -30, -30, -40, -40, -30, -30, -20 },
                    { -10, -20, -20, -20, -20, -20, -20, -10 },
                    {  20,  20,   0,   0,   0,   0,  20,  20 },
                    {  30,  40,  20,   0,   0,  20,  40,  30 },

                    };  

int white_king_endgame[8][8] = {

                    { -72, -48, -36, -24, -24, -36, -48, -72 },
                    { -48, -24, -12,   0,   0, -12, -24, -48 },
                    { -36, -12,   0,  12,  12,   0, -12, -36 },
                    { -24,   0,  12,  24,  24,  12,   0, -24 },
                    { -24,   0,  12,  24,  24,  12,   0, -24 },
                    { -36, -12,   0,  12,  12,   0, -12, -36 },
                    { -48, -24, -12,   0,   0, -12, -24, -48 },
                    { -72, -48, -36, -24, -24, -36, -48, -72 },
                    
                    };

//for black:
int black_pawn[8][8] = {

                    {  0,   0,	 0,   0,   0,	0,   0,	  0 },
                    { 10,  10,	 5, -15, -15,   5,  10,	 10 },
                    {  5,   0,	 0,   5,   5,	0,   0,	  5 },
                    {  0,   0,	10,  20,  20,  10,   0,	  0 },
                    {  5,   5,	 5,  10,  10,   5,   5,	  5 },
                    { 10,  10,	10,  20,  20,  10,  10,	 10 },
                    { 20,  20,	20,  30,  30,  20,  20,	 20 },
                    {  0,   0,   0,   0,   0,   0,   0,	  0 },	

                    };

int black_knight[8][8] = {

                    { -10, -30, -10, -10, -10, -10, -30, -10 },
                    { -10,   0,   0,   0,   0,   0,   0, -10 },
                    { -10,   0,   5,   5,   5,   5,   0, -10 },                  
                    { -10,   0,   5,  10,  10,   5,   0, -10 },
                    { -10,   0,   5,  10,  10,   5,   0, -10 },
                    { -10,   0,   5,   5,   5,   5,   0, -10 },
                    { -10,   0,   0,   0,   0,   0,   0, -10 },	
                    { -10, -10, -10, -10, -10, -10, -10, -10 },	
                    
                    };

int black_bishop[8][8] = {

                    { -10, -10, -20, -10, -10, -20, -10, -10 },
                    { -10,   0,   0,   0,   0,   0,   0, -10 },
                    { -10,   0,   5,   5,   5,   5,   0, -10 },                  
                    { -10,   0,   5,  10,  10,   5,   0, -10 },
                    { -10,   0,   5,  10,  10,   5,   0, -10 },
                    { -10,   0,   5,   5,   5,   5,   0, -10 },
                    { -10,   0,   0,   0,   0,   0,   0, -10 },	
                    { -10, -10, -10, -10, -10, -10, -10, -10 },

                    };

int black_rook[8][8] = {

                    {  0,   0,   0,   2,   2,   0,   0,   0 },	
                    { -5,   0,   0,   0,   0,   0,   0,  -5 },                       
                    { -5,   0,   0,   0,   0,   0,   0,  -5 },
                    { -5,   0,   0,   0,   0,   0,   0,  -5 },
                    { -5,   0,   0,   0,   0,   0,   0,  -5 },
                    { -5,   0,   0,   0,   0,   0,   0,  -5 }, 
                    { 20,  20,  20,  20,  20,  20,  20,  20 },
                    {  5,   5,   5,   5,   5,   5,   5,   5 },

                    };

int black_queen[8][8] = {

                    { -20, -10, -10,  -5,  -5, -10, -10, -20 },
                    { -10,   0,   5,   0,   0,   0,   0, -10 },
                    { -10,   5,   5,   5,   5,   5,   0, -10 },
                    {   0,   0,   5,   5,   5,   5,   0,  -5 },
                    {  -5,   0,   5,   5,   5,   5,   0,  -5 },
                    { -10,   0,   5,   5,   5,   5,   0, -10 },
                    { -10,   0,   0,   0,   0,   0,   0, -10 },
                    { -20, -10, -10,  -5,  -5, -10, -10, -20 },

                    };

int black_king_midgame[8][8] = {

                    {  30,  40,  20,   0,   0,  20,  40,  30 },
                    {  20,  20,   0,   0,   0,   0,  20,  20 },
                    { -10, -20, -20, -20, -20, -20, -20, -10 },     
                    { -20, -30, -30, -40, -40, -30, -30, -20 },
                    { -30, -40, -40, -50, -50, -40, -40, -30 },
                    { -30, -40, -40, -50, -50, -40, -40, -30 },
                    { -30, -40, -40, -50, -50, -40, -40, -30 },
                    { -30, -40, -40, -50, -50, -40, -40, -30 },

                    };      

int black_king_endgame[8][8] = {

                    { -72, -48, -36, -24, -24, -36, -48, -72 },
                    { -48, -24, -12,   0,   0, -12, -24, -48 },
                    { -36, -12,   0,  12,  12,   0, -12, -36 },
                    { -24,   0,  12,  24,  24,  12,   0, -24 },
                    { -24,   0,  12,  24,  24,  12,   0, -24 },
                    { -36, -12,   0,  12,  12,   0, -12, -36 },
                    { -48, -24, -12,   0,   0, -12, -24, -48 },
                    { -72, -48, -36, -24, -24, -36, -48, -72 },
                    
                    };

static inline int wpawn_controlled(char board[8][8], int x, int y)
{
    int squares = 0;
    if(x < 6)
    {
        switch(y)
        {
            case 0:
                if(board[x+1][y+1] == 'P')
                {
                    squares++;
                }
                break;
            case 7:
                if(board[x+1][y-1] == 'P')
                {
                    squares++;
                }
                break;
            default:
                if(board[x+1][y-1] == 'P')
                {
                    squares++;
                }
                if(board[x+1][y+1] == 'P')
                {
                    squares++;
                }
                break;
        }
    }
    return squares;
}

static inline int bpawn_controlled(char board[8][8], int x, int y)
{
    int squares = 0;
    if(x > 1)
    {
        switch(y)
        {
            case 0:
                if(board[x-1][y+1] == 'p')
                {
                    squares++;;
                }
                break;
            case 7:
                if(board[x-1][y-1] == 'p')
                {
                    squares++;;
                }
                break;
            default:
                if(board[x-1][y-1] == 'p')
                {
                    squares++;
                }
                if(board[x-1][y+1] == 'p')
                {
                    squares++;
                }
                break;
        }
    }
    return squares;
}

static inline bool candidate_passed_white(char board[8][8], int x, int y)
{
    for(int rank = 1; rank < x; rank++)
    {
        //don't count as passed when there's a doubled pawn
        if(toupper(board[rank][y]) == 'P')
        {
            return false;
        }
        if(bpawn_controlled(board, rank, y) > wpawn_controlled(board, rank, y))
        {
            return false;
        }
    }
    return true;    
}

static inline bool candidate_passed_black(char board[8][8], int x, int y)
{
    for(int rank = 6; rank > x; rank--)
    {
        //don't count as passed when there's a doubled pawn
        if(toupper(board[rank][y]) == 'P')
        {
            return false;
        }
        if(wpawn_controlled(board, rank, y) > bpawn_controlled(board, rank, y))
        {
            return false;
        }
    }
    return true;    
}

static inline bool connected_white(char board[8][8], int x, int y)
{
    switch(y)
    {
        case 0:
            if(board[x+1][y+1] == 'P')
            {
                return true;
            }
            break;
        case 7:
            if(board[x+1][y-1] == 'P')
            {
                return true;
            }
            break; 
        default:
            if(board[x+1][y-1] == 'P' || board[x+1][y+1] == 'P')
            {
                return true;
            }
            break;

    }
    return false;    
}

static inline bool connected_black(char board[8][8], int x, int y)
{
    switch(y)
    {
        case 0:
            if(board[x-1][y+1] == 'p')
            {
                return true;
            }
            break;
        case 7:
            if(board[x-1][y-1] == 'p')
            {
                return true;
            }
            break; 
        default:
            if(board[x-1][y+1] == 'p' || board[x-1][y-1] == 'p')
            {
                return true;
            }
            break;

    }
    return false;    
}

static inline bool phalanx_white(char board[8][8], int x, int y)
{
    switch(y)
    {
        case 0:
            if(board[x][y+1] == 'P')
            {
                return true;
            }
            break;
        case 7:
            if(board[x][y-1] == 'P')
            {
                return true;
            }
            break; 
        default:
            if(board[x][y-1] == 'P' || board[x][y+1] == 'P')
            {
                return true;
            }
            break;

    }
    return false;    
}

static inline bool phalanx_black(char board[8][8], int x, int y)
{
    switch(y)
    {
        case 0:
            if(board[x][y+1] == 'p')
            {
                return true;
            }
            break;
        case 7:
            if(board[x][y-1] == 'p')
            {
                return true;
            }
            break; 
        default:
            if(board[x][y+1] == 'p' || board[x][y-1] == 'p')
            {
                return true;
            }
            break;

    }
    return false;    
}

static inline bool isolated_white(char board[8][8], int y)
{
    switch(y)
    {
        case 0:
            for(int rank = 6; rank > 0; rank--)
            {
                if(board[rank][1] == 'P')
                {
                    return false;
                }
            }
            break;
        case 7:
            for(int rank = 6; rank > 0; rank--)
            {
                if(board[rank][6] == 'P')
                {
                    return false;
                }
            }
            break; 
        default:
            for(int rank = 6; rank > 0; rank--)
            {
                if(board[rank][y-1] == 'P')
                {
                    return false;
                }
                if(board[rank][y+1] == 'P')
                {
                    return false;
                }
            }
            break;
    }
    return true;    
}

static inline bool isolated_black(char board[8][8], int y)
{
    switch(y)
    {
        case 0:
            for(int rank = 1; rank < 7; rank++)
            {
                if(board[rank][1] == 'p')
                {
                    return false;
                }
            }
            break;
        case 7:
            for(int rank = 1; rank < 7; rank++)
            {
                if(board[rank][6] == 'p')
                {
                    return false;
                }
            }
            break;   
        default:
            for(int rank = 1; rank < 7; rank++)
            {
                if(board[rank][y-1] == 'p')
                {
                    return false;
                }
                if(board[rank][y+1] == 'p')
                {
                    return false;
                }
            }
            break;  
    }
    return true;    
}

static inline bool backward_white(char board[8][8], int x, int y)
{
    switch(y)
    {
        case 0:
            for(int rank = 6; rank >= x; rank--)
            {
                if(board[rank][1] == 'P')
                {
                    return false;
                }
            }
            break;
        case 7:
            for(int rank = 6; rank >= x; rank--)
            {
                if(board[rank][6] == 'P')
                {
                    return false;
                }
            }
            break;    
        default:
            for(int rank = 6; rank >= x; rank--)
            {
                if(board[rank][y-1] == 'P')
                {
                    return false;
                }
                if(board[rank][y+1] == 'P')
                {
                    return false;
                }
            }
            break; 
    }
    return true;    
}

static inline bool backward_black(char board[8][8], int x, int y)
{
    switch(y)
    {
        case 0:
            for(int rank = 1; rank <= x; rank++)
            {
                if(board[rank][1] == 'p')
                {
                    return false;
                }
            }
            break;
        case 7:
            for(int rank = 1; rank <= x; rank++)
            {
                if(board[rank][6] == 'p')
                {
                    return false;
                }
            }
            break;   
        default:
            for(int rank = 1; rank <= x; rank++)
            {
                if(board[rank][y-1] == 'p')
                {
                    return false;
                }
                if(board[rank][y+1] == 'p')
                {
                    return false;
                }
            }
            break;  
    }
    return true;    
}

static inline bool openFile(char board[8][8], int y)
{
    for(int x = 1; x < 7; x++)
    {
        if(tolower(board[x][y]) == 'p')
        {
            return false;
        }
    }
    return true;
}

static inline bool queenFile(BOARD *pos, int y)
{
    for(int i = 0; i < pos->piece_count[wQ]; i++)
    {
        if(y == (pos->piece_list[wQ][i] % 8))
        {
            return true;
        }
    }
    for(int i = 0; i < pos->piece_count[bQ]; i++)
    {
        if(y == (pos->piece_list[bQ][i] % 8))
        {
            return true;
        }
    }
    return false;
}

static inline bool semiOpenFile_white(char board[8][8], int y)
{
    for(int x = 6; x > 0; x--)
    {
        if(board[x][y] == 'P')
        {
            return false;
        }
    }
    return true;
}

static inline bool semiOpenFile_black(char board[8][8], int y)
{
    for(int x = 1; x < 7; x++)
    {
        if(board[x][y] == 'p')
        {
            return false;
        }
    }
    return true;
}

static inline bool outpost_white(char board[8][8], int x, int y)
{
    if(x < 5 && x > 1)
    {
        switch(y)
        {
            case 0:
                if(board[x+1][y+1] == 'P')
                {
                    return true;
                }
                break;
            case 7:
                if(board[x+1][y-1] == 'P')
                {
                    return true;
                }
                break;
            default:
                if(board[x+1][y-1] == 'P' || board[x+1][y+1] == 'P')
                {
                    return true;
                }
                break;
        }
    }
    
    return false;
}

static inline bool outpost_black(char board[8][8], int x, int y)
{
    if(x > 2 && x < 6)
    {
        switch(y)
        {
            case 0:
                if(board[x-1][y+1] == 'p')
                {
                    return true;
                }
                break;
            case 7:
                if(board[x-1][y-1] == 'p')
                {
                    return true;
                }
                break;
            default:
                if(board[x-1][y-1] == 'p' || board[x-1][y+1] == 'p')
                {
                    return true;
                }
                break;
        }
    }
    
    return false;
}

static inline bool badBishop_white(char board[8][8], int x, int y)
{
    if(x > 1)
    {
        switch(y)
        {
            case 0:
                if(board[x-1][y+1] == 'P')
                {
                    return true;
                }
                break;
            case 7:
                if(board[x-1][y-1] == 'P')
                {
                    return true;
                }
                break;
            default:
                if(board[x-1][y-1] == 'P' || board[x-1][y+1] == 'P')
                {
                    return true;
                }
                break;
        }
    }
    
    return false;
}

static inline bool badBishop_black(char board[8][8], int x, int y)
{
    if(x < 6)
    {
        switch(y)
        {
            case 0:
                if(board[x+1][y+1] == 'p')
                {
                    return true;
                }
                break;
            case 7:
                if(board[x+1][y-1] == 'p')
                {
                    return true;
                }
                break;
            default:
                if(board[x+1][y-1] == 'p' || board[x+1][y+1] == 'p')
                {
                    return true;
                }
                break;
        }
    }
    
    return false;
}

static inline bool wTarrasch_wrook(BOARD *pos, int x, int y)
{
    for(int i = 0; i < pos->piece_count[wR]; i++)
    {
        if(x < (pos->piece_list[wR][i] / 8) && y == (pos->piece_list[wR][i] % 8))
        {
            return true;
        }
    }
    return false;
}

static inline bool wTarrasch_brook(BOARD *pos, int x, int y)
{
    for(int i = 0; i < pos->piece_count[bR]; i++)
    {
        if(x < (pos->piece_list[bR][i] / 8) && y == (pos->piece_list[bR][i] % 8))
        {
            return true;
        }
    }
    return false;
}

static inline bool bTarrasch_wrook(BOARD *pos, int x, int y)
{
    for(int i = 0; i < pos->piece_count[wR]; i++)
    {
        if(x > (pos->piece_list[wR][i] / 8) && y == (pos->piece_list[wR][i] % 8))
        {
            return true;
        }
    }
    return false;
}

static inline bool bTarrasch_brook(BOARD *pos, int x, int y)
{
    for(int i = 0; i < pos->piece_count[bR]; i++)
    {
        if(x > (pos->piece_list[bR][i] / 8) && y == (pos->piece_list[bR][i] % 8))
        {
            return true;
        }
    }
    return false;
}

int wking_file(char board[8][8], int y)
{
    int result = 0;
    switch(y)
    {
        case 0:
            if(semiOpenFile_white(board, 0))
            {
                result += 15;
            }
            if(semiOpenFile_white(board, 1))
            {
                result += 15;
            }
            break;
        case 7:
            if(semiOpenFile_white(board, 6))
            {
                result += 15;
            }
            if(semiOpenFile_white(board, 7))
            {
                result += 15;
            }
            break;
        default:
            if(semiOpenFile_white(board, y-1))
            {
                result += 15;
            }
            if(semiOpenFile_white(board, y))
            {
                result += 15;
            }
            if(semiOpenFile_white(board, y+1))
            {
                result += 15;
            }
            break;
    }
    return result;
}

int bking_file(char board[8][8], int y)
{
    int result = 0;
    switch(y)
    {
        case 0:
            if(semiOpenFile_black(board, 0))
            {
                result += 15;
            }
            if(semiOpenFile_black(board, 1))
            {
                result += 15;
            }
            break;
        case 7:
            if(semiOpenFile_black(board, 6))
            {
                result += 15;
            }
            if(semiOpenFile_black(board, 7))
            {
                result += 15;
            }
            break;
        default:
            if(semiOpenFile_black(board, y-1))
            {
                result += 15;
            }
            if(semiOpenFile_black(board, y))
            {
                result += 15;
            }
            if(semiOpenFile_black(board, y+1))
            {
                result += 15;
            }
            break;
    }
    return result;
}

static inline bool defended_by_wpawn(char board[8][8], int x, int y)
{
    if(x < 6)
    {
        switch(y)
        {
            case 0:
                if(board[x+1][y+1] == 'P')
                {
                    return true;
                }
                break;
            case 7:
                if(board[x+1][y-1] == 'P')
                {
                    return true;
                }
                break;
            default:
                if(board[x+1][y-1] == 'P' || board[x+1][y+1] == 'P')
                {
                    return true;
                }
                break;
        }
    }

    return false;
}

static inline bool defended_by_bpawn(char board[8][8], int x, int y)
{
    if(x > 1)
    {
        switch(y)
        {
            case 0:
                if(board[x-1][y+1] == 'p')
                {
                    return true;
                }
                break;
            case 7:
                if(board[x-1][y-1] == 'p')
                {
                    return true;
                }
                break;
            default:
                if(board[x-1][y-1] == 'p' || board[x-1][y+1] == 'p')
                {
                    return true;
                }
                break;
        }
    }

    return false;
}

//get king zone
static inline bool wking_zone(int wkingx, int wkingy, int x, int y)
{
    if(y >= wkingy-1 && y <= wkingy+1 && x >= wkingx-1 && x <= wkingx+1)
    {
        return true;
    }

    return false;
}

static inline bool bking_zone(int bkingx, int bkingy, int x, int y)
{
    if(y >= bkingy-1 && y <= bkingy+1 && x >= bkingx-1 && x <= bkingx+1)
    {
        return true;
    }

    return false;
}

//mobility
//exclude space protected by enemy pawns
//exclude own queen for minor pieces
//exclude own blocked pawns and own pawns on rank 2 and 3
//exclude own king
//also consider king attack
static inline int wknight_mobility(char board[8][8], int index_x, int index_y, int bkingx, int bkingy)
{
    int attack = 0;
    int move_count = 0;
    int x, y;
    int knight_moves_x[8] = {-2, -2, -1, -1,  1,  1,  2,  2};
    int knight_moves_y[8] = {-1,  1, -2,  2, -2,  2, -1,  1};
    for(int j = 0; j < 8; j++)
    {
        x = index_x + knight_moves_x[j];
        y = index_y + knight_moves_y[j];
        if(x & 8 || y & 8) //out of board
        {
            continue;
        }
        if(board[x][y] != 'K' && board[x][y] != 'Q' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' ')))
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(bking_zone(bkingx, bkingy, x, y))
        {
            attack++;
        }
    }
    if(attack)
    {
        wattack_count++;
        wattack_weight += 2 * attack;
    }
    
    return 4 * (move_count - 4);
}

static inline int bknight_mobility(char board[8][8], int index_x, int index_y, int wkingx, int wkingy)
{
    int attack = 0;
    int move_count = 0;
    int x, y;
    int knight_moves_x[8] = {-2, -2, -1, -1,  1,  1,  2,  2};
    int knight_moves_y[8] = {-1,  1, -2,  2, -2,  2, -1,  1};
    for(int j = 0; j < 8; j++)
    {
        x = index_x + knight_moves_x[j];
        y = index_y + knight_moves_y[j];
        if(x & 8 || y & 8) //out of board
        {
            continue;
        }
        if(board[x][y] != 'k' && board[x][y] != 'q' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(wking_zone(wkingx, wkingy, x, y))
        {
            attack++;
        }
    }
    if(attack)
    {
        battack_count++;
        battack_weight += 2 * attack;
    }

    return 4 * (move_count - 4);
}

static inline int wbishop_mobility(char board[8][8], int index_x, int index_y, int bkingx, int bkingy)
{
    int attack = 0;
    int move_count = 0;
    int x;
    int y;

    //up left
    for(int j = 1; j <= 7; j++)
    {
        x = index_x - j;
        y = index_y - j;
        if(x < 0 || y < 0)
        {
            break;
        }
        if(board[x][y] != 'K' && board[x][y] != 'Q' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' ')))
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(bking_zone(bkingx, bkingy, x, y))
        {
            attack++;
        }
        if(board[x][y] != ' ')
        {
            break;
        }
    }
    //up right
    for(int j = 1; j <= 7; j++)
    {
        x = index_x - j;
        y = index_y + j;
        if(x < 0 || y > 7)
        {
            break;
        }
        if(board[x][y] != 'K' && board[x][y] != 'Q' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' ')))
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(bking_zone(bkingx, bkingy, x, y))
        {
            attack++;
        }
        if(board[x][y] != ' ')
        {
            break;
        }
    }
    //down left
    for(int j = 1; j <= 7; j++)
    {
        x = index_x + j;
        y = index_y - j;
        if(x > 7 || y < 0)
        {
            break;
        }
        if(board[x][y] != 'K' && board[x][y] != 'Q' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' ')))
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(bking_zone(bkingx, bkingy, x, y))
        {
            attack++;
        }
        if(board[x][y] != ' ')
        {
            break;
        }
    }
    //down right
    for(int j = 1; j <= 7; j++)
    {
        x = index_x + j;
        y = index_y + j;
        if(x > 7 || y > 7)
        {
            break;
        }
        if(board[x][y] != 'K' && board[x][y] != 'Q' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' ')))
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(bking_zone(bkingx, bkingy, x, y))
        {
            attack++;
        }
        if(board[x][y] != ' ')
        {
            break;
        }
    }
    if(attack)
    {
        wattack_count++;
        wattack_weight += 2 * attack;
    }

    return 3 * (move_count - 7);
}

static inline int bbishop_mobility(char board[8][8], int index_x, int index_y, int wkingx, int wkingy)
{
    int attack = 0;
    int move_count = 0;
    int x;
    int y;
    //up left
    for(int j = 1; j <= 7; j++)
    {
        x = index_x - j;
        y = index_y - j;
        if(x < 0 || y < 0)
        {
            break;
        }
        if(board[x][y] != 'k' && board[x][y] != 'q' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(wking_zone(wkingx, wkingy, x, y))
        {
            attack++;
        }
        if(board[x][y] != ' ')
        {
            break;
        }
    }
    //up right
    for(int j = 1; j <= 7; j++)
    {
        x = index_x - j;
        y = index_y + j;
        if(x < 0 || y > 7)
        {
            break;
        }
        if(board[x][y] != 'k' && board[x][y] != 'q' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(wking_zone(wkingx, wkingy, x, y))
        {
            attack++;
        }
        if(board[x][y] != ' ')
        {
            break;
        }
    }
    //down left
    for(int j = 1; j <= 7; j++)
    {
        x = index_x + j;
        y = index_y - j;
        if(x > 7 || y < 0)
        {
            break;
        }
        if(board[x][y] != 'k' && board[x][y] != 'q' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(wking_zone(wkingx, wkingy, x, y))
        {
            attack++;
        }
        if(board[x][y] != ' ')
        {
            break;
        }
    }
    //down right
    for(int j = 1; j <= 7; j++)
    {
        x = index_x + j;
        y = index_y + j;
        if(x > 7 || y > 7)
        {
            break;
        }
        if(board[x][y] != 'k' && board[x][y] != 'q' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(wking_zone(wkingx, wkingy, x, y))
        {
            attack++;
        }
        if(board[x][y] != ' ')
        {
            break;
        }
    }
    if(attack)
    {
        battack_count++;
        battack_weight += 2 * attack;
    }

    return 3 * (move_count - 7);
}

static inline int wrook_mobility(char board[8][8], int index_x, int index_y, int bkingx, int bkingy)
{
    int attack = 0;
    int move_count = 0;
    int x;
    int y;
    //up
    for(int j = index_x-1; j >= 0; j--)
    {
        x = j;
        y = index_y;
        if(board[x][y] != 'K' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' '))) 
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(bking_zone(bkingx, bkingy, x, y))
        {
            attack++;
        }
        if(board[x][y] != ' ')
        {
            break;
        }
    }
    //left
    for(int j = index_y-1; j >= 0; j--)
    {
        x = index_x;
        y = j;
        if(board[x][y] != 'K' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' '))) 
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(bking_zone(bkingx, bkingy, x, y))
        {
            attack++;
        }
        if(board[x][y] != ' ')
        {
            break;
        }
    }
    //right
    for(int j = index_y+1; j <= 7; j++)
    {
        x = index_x;
        y = j;
        if(board[x][y] != 'K' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' ')))   
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(bking_zone(bkingx, bkingy, x, y))
        {
            attack++;
        }
        if(board[x][y] != ' ')
        {
            break;
        }
    }
    //down
    for(int j = index_x+1; j <= 7; j++)
    {
        x = j;
        y = index_y;
        if(board[x][y] != 'K' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' '))) 
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(bking_zone(bkingx, bkingy, x, y))
        {
            attack++;
        }
        if(board[x][y] != ' ')
        {
            break;
        }
    }
    if(attack)
    {
        wattack_count++;
        wattack_weight += 3 * attack;
    }

    return 2 * (move_count - 7);
}

static inline int brook_mobility(char board[8][8], int index_x, int index_y, int wkingx, int wkingy)
{
    int attack = 0;
    int move_count = 0;
    int x;
    int y;
    //up
    for(int j = index_x-1; j >= 0; j--)
    {  
        x = j;
        y = index_y;
        if(board[x][y] != 'k' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(wking_zone(wkingx, wkingy, x, y))
        {
            attack++;
        }
        if(board[x][y] != ' ')
        {
            break;
        }
    }
    //left
    for(int j = index_y-1; j >= 0; j--)
    {
        x = index_x;
        y = j;
        if(board[x][y] != 'k' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(wking_zone(wkingx, wkingy, x, y))
        {
            attack++;
        }
        if(board[x][y] != ' ')
        {
            break;
        }
    }
    //right
    for(int j = index_y+1; j <= 7; j++)
    {
        x = index_x;
        y = j;
        if(board[x][y] != 'k' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(wking_zone(wkingx, wkingy, x, y))
        {
            attack++;
        }
        if(board[x][y] != ' ')
        {
            break;
        }
    }
    //down
    for(int j = index_x+1; j <= 7; j++)
    {
        x = j;
        y = index_y;
        if(board[x][y] != 'k' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(wking_zone(wkingx, wkingy, x, y))
        {
            attack++;
        }
        if(board[x][y] != ' ')
        {
            break;
        }
    }
    if(attack)
    {
        battack_count++;
        battack_weight += 3 * attack;
    }

    return 2 * (move_count - 7);
}

static inline int wqueen_mobility(char board[8][8], int index_x, int index_y, int bkingx, int bkingy)
{
    int attack = 0;
    int move_count = 0;
    int x;
    int y;
    //up left
    for(int j = 1; j <= 7; j++)
    {
        x = index_x - j;
        y = index_y - j;
        if(x < 0 || y < 0)
        {
            break;
        }
        if(board[x][y] != 'K' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' '))) 
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(bking_zone(bkingx, bkingy, x, y))
        {
            attack++;
        }
        if(board[x][y] != ' ')
        {
            break;
        }
    }
    //up
    for(int j = index_x-1; j >= 0; j--)
    {
        x = j;
        y = index_y;
        if(board[x][y] != 'K' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' '))) 
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(bking_zone(bkingx, bkingy, x, y))
        {
            attack++;
        }
        if(board[x][y] != ' ')
        {
            break;
        }
    }
    //up right
    for(int j = 1; j <= 7; j++)
    {
        x = index_x - j;
        y = index_y + j;
        if(x < 0 || y > 7)
        {
            break;
        }
        if(board[x][y] != 'K' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' '))) 
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(bking_zone(bkingx, bkingy, x, y))
        {
            attack++;
        }
        if(board[x][y] != ' ')
        {
            break;
        }
    }
    //left
    for(int j = index_y-1; j >= 0; j--)
    {
        x = index_x;
        y = j;
        if(board[x][y] != 'K' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' '))) 
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(bking_zone(bkingx, bkingy, x, y))
        {
            attack++;
        }
        if(board[x][y] != ' ')
        {
            break;
        }
    }
    //right
    for(int j = index_y+1; j <= 7; j++)
    {
        x = index_x;
        y = j;
        if(board[x][y] != 'K' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' ')))   
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(bking_zone(bkingx, bkingy, x, y))
        {
            attack++;
        }
        if(board[x][y] != ' ')
        {
            break;
        }
    }
    //down left
    for(int j = 1; j <= 7; j++)
    {
        x = index_x + j;
        y = index_y - j;
        if(x > 7 || y < 0)
        {
            break;
        }
        if(board[x][y] != 'K' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' '))) 
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(bking_zone(bkingx, bkingy, x, y))
        {
            attack++;
        }
        if(board[x][y] != ' ')
        {
            break;
        }
    }
    //down
    for(int j = index_x+1; j <= 7; j++)
    {
        x = j;
        y = index_y;
        if(board[x][y] != 'K' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' '))) 
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(bking_zone(bkingx, bkingy, x, y))
        {
            attack++;
        }
        if(board[x][y] != ' ')
        {
            break;
        }
    }
    //down right
    for(int j = 1; j <= 7; j++)
    {
        x = index_x + j;
        y = index_y + j;
        if(x > 7 || y > 7)
        {
            break;
        }
        if(board[x][y] != 'K' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' '))) 
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(bking_zone(bkingx, bkingy, x, y))
        {
            attack++;
        }
        if(board[x][y] != ' ')
        {
            break;
        }
    }
    if(attack)
    {
        wattack_count++;
        wattack_weight += 4 * attack;
    }

    return 1 * (move_count - 14);
}

static inline int bqueen_mobility(char board[8][8], int index_x, int index_y, int wkingx, int wkingy)
{
    int attack = 0;
    int move_count = 0;
    int x;
    int y;
    //up left
    for(int j = 1; j <= 7; j++)
    {
        x = index_x - j;
        y = index_y - j;
        if(x < 0 || y < 0)
        {
            break;
        }
        if(board[x][y] != 'k' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(wking_zone(wkingx, wkingy, x, y))
        {
            attack++;
        }
        if(board[x][y] != ' ')
        {
            break;
        }
    }
    //up
    for(int j = index_x-1; j >= 0; j--)
    {
        x = j;
        y = index_y;
        if(board[x][y] != 'k' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(wking_zone(wkingx, wkingy, x, y))
        {
            attack++;
        }
        if(board[x][y] != ' ')
        {
            break;
        }
    }
    //up right
    for(int j = 1; j <= 7; j++)
    {
        x = index_x - j;
        y = index_y + j;
        if(x < 0 || y > 7)
        {
            break;
        }
        if(board[x][y] != 'k' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(wking_zone(wkingx, wkingy, x, y))
        {
            attack++;
        }
        if(board[x][y] != ' ')
        {
            break;
        }
    }
    //left
    for(int j = index_y-1; j >= 0; j--)
    {
        x = index_x;
        y = j;
        if(board[x][y] != 'k' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(wking_zone(wkingx, wkingy, x, y))
        {
            attack++;
        }
        if(board[x][y] != ' ')
        {
            break;
        }
    }
    //right
    for(int j = index_y+1; j <= 7; j++)
    {
        x = index_x;
        y = j;
        if(board[x][y] != 'k' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(wking_zone(wkingx, wkingy, x, y))
        {
            attack++;
        }
        if(board[x][y] != ' ')
        {
            break;
        }
    }
    //down left
    for(int j = 1; j <= 7; j++)
    {
        x = index_x + j;
        y = index_y - j;
        if(x > 7 || y < 0)
        {
            break;
        }
        if(board[x][y] != 'k' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(wking_zone(wkingx, wkingy, x, y))
        {
            attack++;
        }
        if(board[x][y] != ' ')
        {
            break;
        }
    }
    //down
    for(int j = index_x+1; j <= 7; j++)
    {
        x = j;
        y = index_y;
        if(board[x][y] != 'k' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(wking_zone(wkingx, wkingy, x, y))
        {
            attack++;
        }
        if(board[x][y] != ' ')
        {
            break;
        }
    }
    //down right
    for(int j = 1; j <= 7; j++)
    {
        x = index_x + j;
        y = index_y + j;
        if(x > 7 || y > 7)
        {
            break;
        }
        if(board[x][y] != 'k' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(wking_zone(wkingx, wkingy, x, y))
        {
            attack++;
        }
        if(board[x][y] != ' ')
        {
            break;
        }
    }
    if(attack)
    {
        battack_count++;
        battack_weight += 4 * attack;
    }

    return 1 * (move_count - 14);
}

//1: black
//-1: white
int evaluate(BOARD *pos, char board[8][8], int color)
{
    //tt probe
    struct Eval* entry = probeEvalTT(pos->key);
    if(entry != NULL)
    { 
        return entry->evaluation;
    }

    int P_count = pos->piece_count[wP], R_count = pos->piece_count[wR], N_count = pos->piece_count[wN]; 
    int B_count = pos->piece_count[wB], Q_count = pos->piece_count[wQ]; // White Chess piece counts
    int p_count = pos->piece_count[bP], r_count = pos->piece_count[bR], n_count = pos->piece_count[bN];
    int b_count = pos->piece_count[bB], q_count = pos->piece_count[bQ]; // Black Chess piece counts
    int points = 0;
    int position_bonus_white = 0;
    int position_bonus_black = 0;
    int other_bonus_white = 0;
    int other_bonus_black = 0;
    int white_king_x = pos->piece_list[wK][0] / 8;
    int white_king_y = pos->piece_list[wK][0] % 8;
    int black_king_x = pos->piece_list[bK][0] / 8;
    int black_king_y = pos->piece_list[bK][0] % 8;
    int tempo = 0;   
    int midgame_white = 0, midgame_black = 0;
    int endgame_white = 0, endgame_black = 0;
    int wqueen_mob, bqueen_mob, wrook_mob, brook_mob;
    bool passed;
    int wqueen_tropism, bqueen_tropism, wrook_tropism, brook_tropism;
    int wknight_tropism, bknight_tropism, wbishop_tropism, bbishop_tropism;
    int pawn_white = 0, pawn_black = 0;
    int x, y;
    wattack_count = battack_count = wattack_weight = battack_weight = 0;
    
    //white pawn
    for(int i = 0; i < P_count; i++)
    {
        x = pos->piece_list[wP][i] / 8;
        y = pos->piece_list[wP][i] % 8;

        passed = false;
        if(board[x-1][y] == 'P')
        {
            pawn_white -= DOUBLEDPAWN;     //doubled pawn penalty    
            pawn_white += white_pawn[x][y];
        }
        else if(candidate_passed_white(board, x, y))
        {
            passed = true;
            //blockage penalty
            if(islower(board[x-1][y]))
                pawn_white += PawnPassed_white[x+1];
            else
                pawn_white += PawnPassed_white[x];
            if(phalanx_white(board, x, y))
            {
                pawn_white += 20;
            }
            //Tarrasch rule
            if(wTarrasch_wrook(pos, x, y))
            {
                endgame_white += 15;
            }
            if(wTarrasch_brook(pos, x, y))
            {
                endgame_white -= 15;
            }
            //king_proximity
            endgame_white -= (abs(x - white_king_x) + abs(y - white_king_y) - 7)*3;
            endgame_white += (abs(x - black_king_x) + abs(y - black_king_y) - 7)*3;
        }
        else
        {
            pawn_white += white_pawn[x][y];
        }

        if(connected_white(board, x, y))
        {
            if(passed)
                pawn_white += CONNECTEDPAWN*2;
            else
                pawn_white += CONNECTEDPAWN;    //connected pawn bonus
        }
        else if(isolated_white(board, y))
        {
            pawn_white -= ISOLATEDPAWN;
        }
        
        if(backward_white(board, x, y))
        {
            pawn_white -= BACKWARDPAWN;                        
        }      
    }
    //black pawn
    for(int i = 0; i < p_count; i++)
    {
        x = pos->piece_list[bP][i] / 8;
        y = pos->piece_list[bP][i] % 8;

        passed = false;
        if(board[x+1][y] == 'p')
        {
            pawn_black -= DOUBLEDPAWN;     //doubled pawn penalty
            pawn_black += black_pawn[x][y];
        }
        else if(candidate_passed_black(board, x, y))
        {
            passed = true;
            //blockage penalty
            if(isupper(board[x+1][y]))
                pawn_black += PawnPassed_black[x-1];
            else
                pawn_black += PawnPassed_black[x];
            if(phalanx_black(board, x, y))
            {
                pawn_black += 20;    //connected pawn bonus
            }
            //Tarrasch rule
            if(bTarrasch_brook(pos, x, y))
            {
                endgame_black += 15;
            }
            if(bTarrasch_wrook(pos, x, y))
            {
                endgame_black -= 15;
            }
            //king_proximity
            endgame_black -= (abs(x - black_king_x) + abs(y - black_king_y) - 7)*3;
            endgame_black += (abs(x - white_king_x) + abs(y - white_king_y) - 7)*3;
        }
        else
        {
            pawn_black += black_pawn[x][y];
        }
        
        if(connected_black(board, x, y))
        {
            if(passed)
                pawn_black += CONNECTEDPAWN*2;    //connected pawn bonus
            else
                pawn_black += CONNECTEDPAWN; 
        }
        else if(isolated_black(board, y))
        {
            pawn_black -= ISOLATEDPAWN;
        }

        if(backward_black(board, x, y))
        {
            pawn_black -= BACKWARDPAWN;                        
        }
    }
    //white rook
    for(int i = 0; i < R_count; i++)
    {
        x = pos->piece_list[wR][i] / 8;
        y = pos->piece_list[wR][i] % 8;

        position_bonus_white += white_rook[x][y];
        if(openFile(board, y))
        {
            other_bonus_white += ROOKOPENFILE;
        }
        else if(semiOpenFile_white(board, y))
        {
            other_bonus_white += ROOKHALFFILE;
        }

        if(queenFile(pos, y))
        {
            other_bonus_white += ROOKQUEENFILE;
        }
        //mobility
        wrook_mob = wrook_mobility(board, x, y, black_king_x, black_king_y);
        midgame_white += wrook_mob;
        endgame_white += wrook_mob*2;
        //king tropism
        wrook_tropism = abs(x - black_king_x) + abs(y - black_king_y) - 7;
        midgame_black += wrook_tropism*2; 
        endgame_black += wrook_tropism*1; 
    }
    //white knight
    for(int i = 0; i < N_count; i++)
    {
        x = pos->piece_list[wN][i] / 8;
        y = pos->piece_list[wN][i] % 8;

        position_bonus_white += white_knight[x][y];
        if(outpost_white(board, x, y))
        {
            other_bonus_white += OUTPOST; //outpost bonus
        }
        //mobility
        other_bonus_white += wknight_mobility(board, x, y, black_king_x, black_king_y);
        //king tropism
        wknight_tropism = abs(x - black_king_x) + abs(y - black_king_y) - 7;
        midgame_black += wknight_tropism*3; 
        endgame_black += wknight_tropism*3; 
        //trapped pieces penalty
        switch(8*x + y)
        {
            case a8:
                if(board[1][0] == 'p' || board[1][2] == 'p')
                {
                    other_bonus_white -= 100;
                }
                break;
            case h8:
                if(board[1][7] == 'p' || board[1][5] == 'p')
                {
                    other_bonus_white -= 100;
                }
                break;
            case a7:
                if(board[2][0] == 'p' && board[1][1] == 'p')
                {
                    other_bonus_white -= 100;
                }
                break;
            case h7:
                if(board[2][7] == 'p' && board[1][6] == 'p')
                {
                    other_bonus_white -= 100;
                }
                break;
            case c3:
                if(board[6][2] == 'P' && board[4][3] == 'P' && board[4][4] != 'P')
                {
                    other_bonus_white -= 5; //c3 knight penalty
                }
                break;
        }
    }
    //bishop
    for(int i = 0; i < B_count; i++)
    {
        x = pos->piece_list[wB][i] / 8;
        y = pos->piece_list[wB][i] % 8;

        position_bonus_white += white_bishop[x][y];
        if(badBishop_white(board, x, y))
        {
            other_bonus_white -= BADBISHOP;    //bad bishop penalty
        }
        if(outpost_white(board, x, y))
        {
            other_bonus_white += OUTPOST; //outpost bonus
        }
        //mobility
        other_bonus_white += wbishop_mobility(board, x, y, black_king_x, black_king_y);
        //king tropism
        wbishop_tropism = abs(x - black_king_x) + abs(y - black_king_y) - 7;
        midgame_black += wbishop_tropism*2; 
        endgame_black += wbishop_tropism*1; 
        //trapped pieces penalty
        switch(8*x + y)
        {
            case a7:
                if(board[2][1] == 'p' && board[1][2] == 'p')
                {
                    other_bonus_white -= 100;
                }
                break;
            case h7:
                if(board[2][6] == 'p' && board[1][5] == 'p')
                {
                    other_bonus_white -= 100;
                }
                break;
            case a6:
                if(board[3][1] == 'p' && board[2][2] == 'p')
                {
                    other_bonus_white -= 50;
                }
                break;
            case h6:
                if(board[3][6] == 'p' && board[2][5] == 'p')
                {
                    other_bonus_white -= 50;
                }
                break;
            case f1:
                if(board[7][6] == 'K')
                {
                    other_bonus_white += RETURNINGBISHOP; 
                }
                if(board[6][4] == 'P' && board[5][4] != ' ')
                {
                    other_bonus_white -= 20; //bishop no development penalty
                }
                break;
            case c1:
                if(board[7][1] == 'K')
                {
                    other_bonus_white += RETURNINGBISHOP; 
                }
                if(board[6][3] == 'P' && board[5][3] != ' ')
                {
                    other_bonus_white -= 20; //bishop no development penalty
                }
                break;
        }
    }
    //white queen
    for(int i = 0; i < Q_count; i++)
    {
        x = pos->piece_list[wQ][i] / 8;
        y = pos->piece_list[wQ][i] % 8;

        position_bonus_white += white_queen[x][y];
        //queen early development penalty
        if(x < 6)
        {
            if(board[7][1] == 'N')
            {
                other_bonus_white -= 4;
            }
            if(board[7][2] == 'B')
            {
                other_bonus_white -= 4;
            }
            if(board[7][5] == 'B')
            {
                other_bonus_white -= 4;
            }
            if(board[7][6] == 'N')
            {
                other_bonus_white -= 4;
            }
        }
        //mobility
        wqueen_mob = wqueen_mobility(board, x, y, black_king_x, black_king_y);
        midgame_white += wqueen_mob; 
        endgame_white += wqueen_mob*2;
        //king tropism
        wqueen_tropism = abs(x - black_king_x) + abs(y - black_king_y) - 7;
        midgame_black += wqueen_tropism*2; 
        endgame_black += wqueen_tropism*4;
    }
    //black rook
    for(int i = 0; i < r_count; i++)
    {
        x = pos->piece_list[bR][i] / 8;
        y = pos->piece_list[bR][i] % 8;

        position_bonus_black += black_rook[x][y];
        if(openFile(board, y))
        {
            other_bonus_black += ROOKOPENFILE;
        }
        else if(semiOpenFile_black(board, y))
        {
            other_bonus_black += ROOKHALFFILE;
        }

        if(queenFile(pos, y))
        {
            other_bonus_black += ROOKQUEENFILE;
        }
        //mobility
        brook_mob = brook_mobility(board, x, y, white_king_x, white_king_y);
        midgame_black += brook_mob;
        endgame_black += brook_mob*2; 
        //king tropism
        brook_tropism = abs(x - white_king_x) + abs(y - white_king_y) - 7;
        midgame_white += brook_tropism*2; 
        endgame_white += brook_tropism*1;
    }
    //black knight
    for(int i = 0; i < n_count; i++)
    {
        x = pos->piece_list[bN][i] / 8;
        y = pos->piece_list[bN][i] % 8;

        position_bonus_black += black_knight[x][y];
        if(outpost_black(board, x, y))
        {
            other_bonus_black += OUTPOST; //outpost bonus
        }
        //mobility
        other_bonus_black += bknight_mobility(board, x, y, white_king_x, white_king_y);
        //king tropism
        bknight_tropism = abs(x - white_king_x) + abs(y - white_king_y) - 7;
        midgame_white += bknight_tropism*3; 
        endgame_white += bknight_tropism*3;
        //trapped pieces penalty
        switch(8*x + y)
        {
            case a1:
                if(board[6][0] == 'P' || board[6][2] == 'P')
                {
                    other_bonus_black -= 100;
                }
                break;
            case h1:
                if(board[6][7] == 'P' || board[6][5] == 'P')
                {
                    other_bonus_black -= 100;
                }
                break;
            case a2:
                if(board[5][0] == 'P' && board[6][1] == 'P')
                {
                    other_bonus_black -= 100;
                }
                break;
            case h2:
                if(board[5][7] == 'P' && board[6][6] == 'P')
                {
                    other_bonus_black -= 100;
                }
                break;
            case c6:
                if(board[1][2] == 'p' && board[3][3] == 'p' && board[3][4] != 'p')
                {
                    other_bonus_black -= 5; //c6 knight penalty
                }
                break;
        }
    }
    //black bishop
    for(int i = 0; i < b_count; i++)
    {
        x = pos->piece_list[bB][i] / 8;
        y = pos->piece_list[bB][i] % 8;

        position_bonus_black += black_bishop[x][y];
        if(badBishop_black(board, x, y))
        {
            other_bonus_black -= BADBISHOP;    //bad bishop penalty
        }
        if(outpost_black(board, x, y))
        {
            other_bonus_black += OUTPOST; //outpost bonus
        }
        //mobility
        other_bonus_black += bbishop_mobility(board, x, y, white_king_x, white_king_y);
        //king tropism
        bbishop_tropism = abs(x - white_king_x) + abs(y - white_king_y) - 7;
        midgame_white += bbishop_tropism*2; 
        endgame_white += bbishop_tropism*1;
        //trapped pieces penalty
        switch(8*x + y)
        {
            case a2:
                if(board[5][1] == 'P' && board[6][2] == 'P')
                {
                    other_bonus_black -= 100;
                }
                break;
            case h2:
                if(board[5][6] == 'P' && board[6][5] == 'P')
                {
                    other_bonus_black -= 100;
                }
                break;
            case a3:
                if(board[4][1] == 'P' && board[5][2] == 'P')
                {
                    other_bonus_black -= 50;
                }
                break;
            case h3:
                if(board[4][6] == 'P' && board[5][5] == 'P')
                {
                    other_bonus_black -= 50;
                }
                break;
            case f8:
                if(board[0][6] == 'k')
                {
                    other_bonus_black += RETURNINGBISHOP; 
                }
                if(board[1][4] == 'p' && board[2][4] != ' ')
                {
                    other_bonus_black -= 20; //bishop no development penalty
                }
                break;
            case c8:
                if(board[0][1] == 'k')
                {
                    other_bonus_black += RETURNINGBISHOP; 
                }
                if(board[1][3] == 'p' && board[2][3] != ' ')
                {
                    other_bonus_black -= 20; //bishop no development penalty
                }
                break;
        }
    }
    //black queen
    for(int i = 0; i < q_count; i++)
    {
        x = pos->piece_list[bQ][i] / 8;
        y = pos->piece_list[bQ][i] % 8;

        position_bonus_black += black_queen[x][y];
        //queen early development penalty
        if(x > 1)
        {
            if(board[0][1] == 'n')
            {
                other_bonus_black -= 4;
            }
            if(board[0][2] == 'b')
            {
                other_bonus_black -= 4;
            }
            if(board[0][5] == 'b')
            {
                other_bonus_black -= 4;
            }
            if(board[0][6] == 'n')
            {
                other_bonus_black -= 4;
            }
        }
        //mobility
        bqueen_mob = bqueen_mobility(board, x, y, white_king_x, white_king_y);
        midgame_black += bqueen_mob;
        endgame_black += bqueen_mob*2;
        //king tropism
        bqueen_tropism = abs(x - white_king_x) + abs(y - white_king_y) - 7;
        midgame_white += bqueen_tropism*2; 
        endgame_white += bqueen_tropism*4; 
    } 

    //game phase based on non-pawn materials
    int phase = N_count + n_count + B_count + b_count + R_count * 2 + r_count * 2 + Q_count * 4 + q_count * 4;

    //tempo
    if(phase > 4)
        tempo = color * TEMPO;

    if(phase > 0)
    {
        //for kings' position bonus midgame
        midgame_white += white_king_midgame[white_king_x][white_king_y];
        midgame_black += black_king_midgame[black_king_x][black_king_y];
        //pawn shield bonus
        if(white_king_x > 5 && white_king_y > 4)
        {
            if(board[7][6] == 'R' || board[7][7] == 'R') //blocked rook penalty
                midgame_white -= 40;
            
            if(board[6][5] == 'P')  
                midgame_white += 15;
            else if(board[5][5] == 'P')
                midgame_white += 10;
            
            if(board[5][5] == 'p')  //pawn storm penalty
                midgame_white -= 10;
            else if(board[4][5] == 'p')
                midgame_white -= 5;
            
            if(board[6][6] == 'P')
                midgame_white += 15;
            else if(board[5][6] == 'P')
                midgame_white += 10;

            if(board[5][6] == 'p')
                midgame_white -= 10;
            else if(board[4][6] == 'p')
                midgame_white -= 5;    
            
            if(board[6][7] == 'P')
                midgame_white += 15;
            else if(board[5][7] == 'P')
                midgame_white += 10;
            
            if(board[5][7] == 'p')
                midgame_white -= 10;
            else if(board[4][7] == 'p')
                midgame_white -= 5;  
        }
        else if(white_king_x > 5 && white_king_y < 3)
        {
            if(board[7][0] == 'R' || board[7][1] == 'R')
                midgame_white -= 40;
            
            if(board[6][0] == 'P')
                midgame_white += 15;
            else if(board[5][0] == 'P')
                midgame_white += 10; 

            if(board[5][0] == 'p')
                midgame_white -= 10;
            else if(board[4][0] == 'p')
                midgame_white -= 5;  
            
            if(board[6][1] == 'P')
                midgame_white += 15;
            else if(board[5][1] == 'P')
                midgame_white += 10;

            if(board[5][1] == 'p')
                midgame_white -= 10;
            else if(board[4][1] == 'p')
                midgame_white -= 5;  
            
            if(board[6][2] == 'P')
                midgame_white += 15;
            else if(board[5][2] == 'P')
                midgame_white += 10;

            if(board[5][2] == 'p')
                midgame_white -= 10;
            else if(board[4][2] == 'p')
                midgame_white -= 5;  
        }

        if(black_king_x < 2 && black_king_y > 4)
        {
            if(board[0][6] == 'r' || board[0][7] == 'r')
                midgame_black -= 40;
            
            if(board[1][5] == 'p')
                midgame_black += 15;
            else if(board[2][5] == 'p')
                midgame_black += 10;

            if(board[2][5] == 'P')
                midgame_black -= 10;
            else if(board[3][5] == 'P')
                midgame_black -= 5;  
            
            if(board[1][6] == 'p')
                midgame_black += 15;
            else if(board[2][6] == 'p')
                midgame_black += 10;
            
            if(board[2][6] == 'P')
                midgame_black -= 10;
            else if(board[3][6] == 'P')
                midgame_black -= 5; 

            if(board[1][7] == 'p')
                midgame_black += 15;
            else if(board[2][7] == 'p')
                midgame_black += 10;
            
            if(board[2][7] == 'P')
                midgame_black -= 10;
            else if(board[3][7] == 'P')
                midgame_black -= 5; 
        }
        else if(black_king_x < 2 && black_king_y < 3)
        {
            if(board[0][0] == 'r' || board[0][1] == 'r')
                midgame_black -= 40;

            if(board[1][0] == 'p')
                midgame_black += 15;
            else if(board[2][0] == 'p')
                midgame_black += 10;
            
            if(board[2][0] == 'P')
                midgame_black -= 10;
            else if(board[3][0] == 'P')
                midgame_black -= 5; 
            
            if(board[1][1] == 'p')
                midgame_black += 15;
            else if(board[2][1] == 'p')
                midgame_black += 10;
            
            if(board[2][1] == 'P')
                midgame_black -= 10;
            else if(board[3][1] == 'P')
                midgame_black -= 5; 
            
            if(board[1][2] == 'p')
                midgame_black += 15;
            else if(board[2][2] == 'p')
                midgame_black += 10;
            
            if(board[2][2] == 'P')
                midgame_black -= 10;
            else if(board[3][2] == 'P')
                midgame_black -= 5; 
        }
        //open files next to king penalty
        midgame_white -= wking_file(board, white_king_y);
        midgame_black -= bking_file(board, black_king_y);
        //unable to castle penalty
        if(!pos->wcastled && !pos->ksw && !pos->qsw)
            midgame_white -= 15;
        if(!pos->bcastled && !pos->ksb && !pos->qsb)
            midgame_black -= 15;
    }

    //king position bonus endgame
    endgame_white += white_king_endgame[white_king_x][white_king_y];
    endgame_black += black_king_endgame[black_king_x][black_king_y];
    
    points = p_count * 100 + r_count * (500 + rook_val[p_count]) + n_count * (320 + knight_val[p_count]) + b_count * 330 + q_count * 900 + position_bonus_black 
            + ((b_count >= 2)? 1 : 0) * BISHOPPAIR - ((n_count >= 2)? 1 : 0) * KNIGHTPAIR - ((r_count >= 2)? 1 : 0) * ROOKPAIR + other_bonus_black + pawn_black
            - P_count * 100 - R_count * (500 + rook_val[P_count]) - N_count * (320 + knight_val[P_count]) - B_count * 330 - Q_count * 900 - position_bonus_white 
            - ((B_count >= 2)? 1 : 0) * BISHOPPAIR + ((N_count >= 2)? 1 : 0) * KNIGHTPAIR + ((R_count >= 2)? 1 : 0) * ROOKPAIR - other_bonus_white - pawn_white + tempo;
    
    //adjust phase score based on materials
    if(phase > 24)
        phase = 24;
    int mg_weight = phase;
    int eg_weight = 24 - mg_weight;
    points += (((midgame_black - midgame_white) * mg_weight + (endgame_black - endgame_white) * eg_weight) / 24);

    //king attack score
    if(wattack_count < 2 || !Q_count)
        wattack_weight = 0;
    if(battack_count < 2 || !q_count)
        battack_weight = 0;
    points += SafetyTable[battack_weight];
    points -= SafetyTable[wattack_weight];

    //material draw
    if(!P_count && !p_count)
    {
        if(!R_count && !r_count && !Q_count && !q_count)
        {
            if(!B_count && !b_count)
            {
                if(N_count < 3 && n_count < 3)
                {
                    //tt store
                    storeEvalTT(pos->key, 0);
                    return 0;
                }
            }
            else if(!N_count && !n_count)
            {
                if(abs(B_count - b_count) < 2)
                {
                    //tt store
                    storeEvalTT(pos->key, 0);
                    return 0;
                }
            }
            else if((N_count < 3 && !B_count) || (B_count == 1 && !N_count))
            {
                if((n_count < 3 && !b_count) || (b_count == 1 && !n_count))
                {
                    //tt store
                    storeEvalTT(pos->key, 0);
                    return 0;
                }
            }
        }
        else if(!Q_count && !q_count)
        {
            if(R_count == 1 && r_count == 1)
            {
                if(((N_count + B_count) < 2 && (n_count + b_count) == 0) || ((N_count + B_count) == 0 && (n_count + b_count) < 2))
                {
                    //tt store
                    storeEvalTT(pos->key, color * points / 2);
                    return color * points / 2;
                }
            }
            else if(R_count == 1 && !r_count)
            {
                if((N_count + B_count == 0) && (((n_count + b_count) == 1) || ((n_count + b_count) == 2))) 
                { 
                    //tt store
                    storeEvalTT(pos->key, color * points / 2);
                    return color * points / 2; 
                }
            }
            else if(r_count == 1 && !R_count)
            {
                if((n_count + b_count == 0) && (((N_count + B_count) == 1) || ((N_count + B_count) == 2))) 
                { 
                    //tt store
                    storeEvalTT(pos->key, color * points / 2);
                    return color * points / 2; 
                }
            }
        }
    }

    //tt store
    storeEvalTT(pos->key, color * points);
    return color * points;
}