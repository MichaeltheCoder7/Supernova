#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>
#include "Evaluate.h"
#include "Search.h"


#define CONNECTEDPAWN   10
#define DOUBLEDPAWN     10
#define ISOLATEDPAWN    20
#define BACKWARDPAWN    8
#define ROOKOPENFILE    10
#define ROOKHALFFILE    5
#define BISHOPPAIR      30
#define BADBISHOP       6
#define OUTPOST         10
#define KNIGHTPAIR      8
#define ROOKPAIR        16
#define RETURNINGBISHOP 30 
#define TEMPO           10 


int PawnPassed_black[8] = { 0, 15, 20, 32, 56, 92, 140, 0 }; 
int PawnPassed_white[8] = { 0, 140, 92, 56, 32, 20, 15, 0 }; 
int knight_val[9] = { -20, -16, -12, -8, -4,  0,  4,  8, 12 };
int rook_val[9] = { 15,  12,   9,  6,  3,  0, -3, -6, -9 };

enum positions {
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1
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
                    {  20,  40,  40,   0,   0,  10,  40,  20 },

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

                    {  20,  40,  40,   0,   0,  10,  40,  20 },
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


static inline bool passed_white(char board[8][8], int x, int y)
{
    switch(y)
    {
        case 0:
            for(int rank = 1; rank < x; rank++)
            {
                for(int file = 0; file < 2; file++)
                {
                    if(board[rank][file] == 'p')
                    {
                        return false;
                    }
                }
            }
            break;
        case 1:
            for(int rank = 1; rank < x; rank++)
            {
                for(int file = 0; file < 3; file++)
                {
                    if(board[rank][file] == 'p')
                    {
                        return false;
                    }
                }
            }
            break;
        case 2:
            for(int rank = 1; rank < x; rank++)
            {
                for(int file = 1; file < 4; file++)
                {
                    if(board[rank][file] == 'p')
                    {
                        return false;
                    }
                }
            }
            break;
        case 3:
            for(int rank = 1; rank < x; rank++)
            {
                for(int file = 2; file < 5; file++)
                {
                    if(board[rank][file] == 'p')
                    {
                        return false;
                    }
                }
            }
            break;
        case 4:
            for(int rank = 1; rank < x; rank++)
            {
                for(int file = 3; file < 6; file++)
                {
                    if(board[rank][file] == 'p')
                    {
                        return false;
                    }
                }
            }
            break;
        case 5:
            for(int rank = 1; rank < x; rank++)
            {
                for(int file = 4; file < 7; file++)
                {
                    if(board[rank][file] == 'p')
                    {
                        return false;
                    }
                }
            }
            break;
        case 6:
            for(int rank = 1; rank < x; rank++)
            {
                for(int file = 5; file < 8; file++)
                {
                    if(board[rank][file] == 'p')
                    {
                        return false;
                    }
                }
            }
            break;
        case 7:
            for(int rank = 1; rank < x; rank++)
            {
                for(int file = 6; file < 8; file++)
                {
                    if(board[rank][file] == 'p')
                    {
                        return false;
                    }
                }
            }
            break;     
    }
    return true;    
}

static inline bool passed_black(char board[8][8], int x, int y)
{
    switch(y)
    {
        case 0:
            for(int rank = 6; rank > x; rank--)
            {
                for(int file = 0; file < 2; file++)
                {
                    if(board[rank][file] == 'P')
                    {
                        return false;
                    }
                }
            }
            break;
        case 1:
            for(int rank = 6; rank > x; rank--)
            {
                for(int file = 0; file < 3; file++)
                {
                    if(board[rank][file] == 'P')
                    {
                        return false;
                    }
                }
            }
            break;
        case 2:
            for(int rank = 6; rank > x; rank--)
            {
                for(int file = 1; file < 4; file++)
                {
                    if(board[rank][file] == 'P')
                    {
                        return false;
                    }
                }
            }
            break;
        case 3:
            for(int rank = 6; rank > x; rank--)
            {
                for(int file = 2; file < 5; file++)
                {
                    if(board[rank][file] == 'P')
                    {
                        return false;
                    }
                }
            }
            break;
        case 4:
            for(int rank = 6; rank > x; rank--)
            {
                for(int file = 3; file < 6; file++)
                {
                    if(board[rank][file] == 'P')
                    {
                        return false;
                    }
                }
            }
            break;
        case 5:
            for(int rank = 6; rank > x; rank--)
            {
                for(int file = 4; file < 7; file++)
                {
                    if(board[rank][file] == 'P')
                    {
                        return false;
                    }
                }
            }
            break;
        case 6:
            for(int rank = 6; rank > x; rank--)
            {
                for(int file = 5; file < 8; file++)
                {
                    if(board[rank][file] == 'P')
                    {
                        return false;
                    }
                }
            }
            break;
        case 7:
            for(int rank = 6; rank > x; rank--)
            {
                for(int file = 6; file < 8; file++)
                {
                    if(board[rank][file] == 'P')
                    {
                        return false;
                    }
                }
            }
            break;     
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
        case 1:
            for(int rank = 6; rank > 0; rank--)
            {
                if(board[rank][0] == 'P')
                {
                    return false;
                }
                if(board[rank][2] == 'P')
                {
                    return false;
                }
            }
            break;
        case 2:
            for(int rank = 6; rank > 0; rank--)
            {
                if(board[rank][1] == 'P')
                {
                    return false;
                }
                if(board[rank][3] == 'P')
                {
                    return false;
                }
            }
            break;
        case 3:
            for(int rank = 6; rank > 0; rank--)
            {
                if(board[rank][2] == 'P')
                {
                    return false;
                }
                if(board[rank][4] == 'P')
                {
                    return false;
                }
            }
            break;
        case 4:
            for(int rank = 6; rank > 0; rank--)
            {
                if(board[rank][3] == 'P')
                {
                    return false;
                }
                if(board[rank][5] == 'P')
                {
                    return false;
                }
            }
            break;
        case 5:
            for(int rank = 6; rank > 0; rank--)
            {
                if(board[rank][4] == 'P')
                {
                    return false;
                }
                if(board[rank][6] == 'P')
                {
                    return false;
                }
            }
            break;
        case 6:
            for(int rank = 6; rank > 0; rank--)
            {
                if(board[rank][5] == 'P')
                {
                    return false;
                }
                if(board[rank][7] == 'P')
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
        case 1:
            for(int rank = 1; rank < 7; rank++)
            {
                if(board[rank][0] == 'p')
                {
                    return false;
                }
                if(board[rank][2] == 'p')
                {
                    return false;
                }
            }
            break;
        case 2:
            for(int rank = 1; rank < 7; rank++)
            {
                if(board[rank][1] == 'p')
                {
                    return false;
                }
                if(board[rank][3] == 'p')
                {
                    return false;
                }
            }
            break;
        case 3:
            for(int rank = 1; rank < 7; rank++)
            {
                if(board[rank][2] == 'p')
                {
                    return false;
                }
                if(board[rank][4] == 'p')
                {
                    return false;
                }
            }
            break;
        case 4:
            for(int rank = 1; rank < 7; rank++)
            {
                if(board[rank][3] == 'p')
                {
                    return false;
                }
                if(board[rank][5] == 'p')
                {
                    return false;
                }
            }
            break;
        case 5:
            for(int rank = 1; rank < 7; rank++)
            {
                if(board[rank][4] == 'p')
                {
                    return false;
                }
                if(board[rank][6] == 'p')
                {
                    return false;
                }
            }
            break;
        case 6:
            for(int rank = 1; rank < 7; rank++)
            {
                if(board[rank][5] == 'p')
                {
                    return false;
                }
                if(board[rank][7] == 'p')
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
        case 1:
            for(int rank = 6; rank >= x; rank--)
            {
                if(board[rank][0] == 'P')
                {
                    return false;
                }
                if(board[rank][2] == 'P')
                {
                    return false;
                }
            }
            break;
        case 2:
            for(int rank = 6; rank >= x; rank--)
            {
                if(board[rank][1] == 'P')
                {
                    return false;
                }
                if(board[rank][3] == 'P')
                {
                    return false;
                }
            }
            break;
        case 3:
            for(int rank = 6; rank >= x; rank--)
            {
                if(board[rank][2] == 'P')
                {
                    return false;
                }
                if(board[rank][4] == 'P')
                {
                    return false;
                }
            }
            break;
        case 4:
            for(int rank = 6; rank >= x; rank--)
            {
                if(board[rank][3] == 'P')
                {
                    return false;
                }
                if(board[rank][5] == 'P')
                {
                    return false;
                }
            }
            break;
        case 5:
            for(int rank = 6; rank >= x; rank--)
            {
                if(board[rank][4] == 'P')
                {
                    return false;
                }
                if(board[rank][6] == 'P')
                {
                    return false;
                }
            }
            break;
        case 6:
            for(int rank = 6; rank >= x; rank--)
            {
                if(board[rank][5] == 'P')
                {
                    return false;
                }
                if(board[rank][7] == 'P')
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
        case 1:
            for(int rank = 1; rank <= x; rank++)
            {
                if(board[rank][0] == 'p')
                {
                    return false;
                }
                if(board[rank][2] == 'p')
                {
                    return false;
                }
            }
            break;
        case 2:
            for(int rank = 1; rank <= x; rank++)
            {
                if(board[rank][1] == 'p')
                {
                    return false;
                }
                if(board[rank][3] == 'p')
                {
                    return false;
                }
            }
            break;
        case 3:
            for(int rank = 1; rank <= x; rank++)
            {
                if(board[rank][2] == 'p')
                {
                    return false;
                }
                if(board[rank][4] == 'p')
                {
                    return false;
                }
            }
            break;
        case 4:
            for(int rank = 1; rank <= x; rank++)
            {
                if(board[rank][3] == 'p')
                {
                    return false;
                }
                if(board[rank][5] == 'p')
                {
                    return false;
                }
            }
            break;
        case 5:
            for(int rank = 1; rank <= x; rank++)
            {
                if(board[rank][4] == 'p')
                {
                    return false;
                }
                if(board[rank][6] == 'p')
                {
                    return false;
                }
            }
            break;
        case 6:
            for(int rank = 1; rank <= x; rank++)
            {
                if(board[rank][5] == 'p')
                {
                    return false;
                }
                if(board[rank][7] == 'p')
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
    }
    return true;    
}

static inline bool openFile(char board[8][8], int y)
{
    for(int x = 1; x < 7; x++)
    {
        if(board[x][y] == 'p')
        {
            return false;
        }
        if(board[7-x][y] == 'P')
        {
            return false;
        }
    }
    return true;
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
    if(x < 5 && x != 0)
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
    if(x > 2 && x != 7)
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
    if(x != 0)
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
    if(x != 7)
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

static inline bool defended_by_wpawn(char board[8][8], int x, int y)
{
    if(x != 7)
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
    if(x != 0)
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

//mobility
//exclude space protected by enemy pawns
//exclude own queen for minor pieces
//exclude own blocked pawns and own pawns on rank 2 and 3
//exclude own king
static inline int wknight_mobility(char board[8][8], int index_x, int index_y)
{
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
    }
    
    return 4 * (move_count - 4);
}

static inline int bknight_mobility(char board[8][8], int index_x, int index_y)
{
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
    }
    return 4 * (move_count - 4);
}

static inline int wbishop_mobility(char board[8][8], int index_x, int index_y)
{
    int move_count = 0;
    int x;
    int y;

    //up left
    for(int j = 1; j <= 7; j++)
    {
        if(index_x - j < 0 || index_y - j < 0)
        {
            break;
        }
        x = index_x - j;
        y = index_y - j;
        if(board[x][y] != 'K' && board[x][y] != 'Q' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' ')))
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(board[index_x - j][index_y - j] != ' ')
        {
            break;
        }
    }
    //up right
    for(int j = 1; j <= 7; j++)
    {
        if(index_x - j < 0 || index_y + j > 7)
        {
            break;
        }
        x = index_x - j;
        y = index_y + j;
        if(board[x][y] != 'K' && board[x][y] != 'Q' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' ')))
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(board[index_x - j][index_y + j] != ' ')
        {
            break;
        }
    }
    //down left
    for(int j = 1; j <= 7; j++)
    {
        if(index_x + j > 7 || index_y - j < 0)
        {
            break;
        }
        x = index_x + j;
        y = index_y - j;
        if(board[x][y] != 'K' && board[x][y] != 'Q' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' ')))
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(board[index_x + j][index_y - j] != ' ')
        {
            break;
        }
    }
    //down right
    for(int j = 1; j <= 7; j++)
    {
        if(index_x + j > 7 || index_y + j > 7)
        {
            break;
        }
        x = index_x + j;
        y = index_y + j;
        if(board[x][y] != 'K' && board[x][y] != 'Q' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' ')))
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(board[index_x + j][index_y + j] != ' ')
        {
            break;
        }
    }
    return 3 * (move_count - 7);
}

static inline int bbishop_mobility(char board[8][8], int index_x, int index_y)
{
    int move_count = 0;
    int x;
    int y;
    //up left
    for(int j = 1; j <= 7; j++)
    {
        if(index_x - j < 0 || index_y - j < 0)
        {
            break;
        }
        x = index_x - j;
        y = index_y - j;
        if(board[x][y] != 'k' && board[x][y] != 'q' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(board[index_x - j][index_y - j] != ' ')
        {
            break;
        }
    }
    //up right
    for(int j = 1; j <= 7; j++)
    {
        if(index_x - j < 0 || index_y + j > 7)
        {
            break;
        }
        x = index_x - j;
        y = index_y + j;
        if(board[x][y] != 'k' && board[x][y] != 'q' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(board[index_x - j][index_y + j] != ' ')
        {
            break;
        }
    }
    //down left
    for(int j = 1; j <= 7; j++)
    {
        if(index_x + j > 7 || index_y - j < 0)
        {
            break;
        }
        x = index_x + j;
        y = index_y - j;
        if(board[x][y] != 'k' && board[x][y] != 'q' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(board[index_x + j][index_y - j] != ' ')
        {
            break;
        }
    }
    //down right
    for(int j = 1; j <= 7; j++)
    {
        if(index_x + j > 7 || index_y + j > 7)
        {
            break;
        }
        x = index_x + j;
        y = index_y + j;
        if(board[x][y] != 'k' && board[x][y] != 'q' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(board[index_x + j][index_y + j] != ' ')
        {
            break;
        }
    }
    return 3 * (move_count - 7);
}

static inline int wrook_mobility(char board[8][8], int index_x, int index_y)
{
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
        if(board[j][index_y] != ' ')
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
        if(board[index_x][j] != ' ')
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
        if(board[index_x][j] != ' ')
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
        if(board[j][index_y] != ' ')
        {
            break;
        }
    }
    return 2 * (move_count - 7);
}

static inline int brook_mobility(char board[8][8], int index_x, int index_y)
{
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
        if(board[j][index_y] != ' ')
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
        if(board[index_x][j] != ' ')
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
        if(board[index_x][j] != ' ')
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
        if(board[j][index_y] != ' ')
        {
            break;
        }
    }
    return 2 * (move_count - 7);
}

static inline int wqueen_mobility(char board[8][8], int index_x, int index_y)
{
    int move_count = 0;
    int x;
    int y;
    //up left
    for(int j = 1; j <= 7; j++)
    {
        if(index_x - j < 0 || index_y - j < 0)
        {
            break;
        }
        x = index_x - j;
        y = index_y - j;
        if(board[x][y] != 'K' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' '))) 
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(board[index_x - j][index_y - j] != ' ')
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
        if(board[j][index_y] != ' ')
        {
            break;
        }
    }
    //up right
    for(int j = 1; j <= 7; j++)
    {
        if(index_x - j < 0 || index_y + j > 7)
        {
            break;
        }
        x = index_x - j;
        y = index_y + j;
        if(board[x][y] != 'K' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' '))) 
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(board[index_x - j][index_y + j] != ' ')
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
        if(board[index_x][j] != ' ')
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
        if(board[index_x][j] != ' ')
        {
            break;
        }
    }
    //down left
    for(int j = 1; j <= 7; j++)
    {
        if(index_x + j > 7 || index_y - j < 0)
        {
            break;
        }
        x = index_x + j;
        y = index_y - j;
        if(board[x][y] != 'K' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' '))) 
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(board[index_x + j][index_y - j] != ' ')
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
        if(board[j][index_y] != ' ')
        {
            break;
        }
    }
    //down right
    for(int j = 1; j <= 7; j++)
    {
        if(index_x + j > 7 || index_y + j > 7)
        {
            break;
        }
        x = index_x + j;
        y = index_y + j;
        if(board[x][y] != 'K' && (board[x][y] != 'P' || (x < 5 && board[x - 1][y] == ' '))) 
        {  
            if(!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if(board[index_x + j][index_y + j] != ' ')
        {
            break;
        }
    }
    return 1 * (move_count - 14);
}

static inline int bqueen_mobility(char board[8][8], int index_x, int index_y)
{
    int move_count = 0;
    int x;
    int y;
    //up left
    for(int j = 1; j <= 7; j++)
    {
        if(index_x - j < 0 || index_y - j < 0)
        {
            break;
        }
        x = index_x - j;
        y = index_y - j;
        if(board[x][y] != 'k' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(board[index_x - j][index_y - j] != ' ')
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
        if(board[j][index_y] != ' ')
        {
            break;
        }
    }
    //up right
    for(int j = 1; j <= 7; j++)
    {
        if(index_x - j < 0 || index_y + j > 7)
        {
            break;
        }
        x = index_x - j;
        y = index_y + j;
        if(board[x][y] != 'k' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(board[index_x - j][index_y + j] != ' ')
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
        if(board[index_x][j] != ' ')
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
        if(board[index_x][j] != ' ')
        {
            break;
        }
    }
    //down left
    for(int j = 1; j <= 7; j++)
    {
        if(index_x + j > 7 || index_y - j < 0)
        {
            break;
        }
        x = index_x + j;
        y = index_y - j;
        if(board[x][y] != 'k' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(board[index_x + j][index_y - j] != ' ')
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
        if(board[j][index_y] != ' ')
        {
            break;
        }
    }
    //down right
    for(int j = 1; j <= 7; j++)
    {
        if(index_x + j > 7 || index_y + j > 7)
        {
            break;
        }
        x = index_x + j;
        y = index_y + j;
        if(board[x][y] != 'k' && (board[x][y] != 'p' || (x > 2 && board[x + 1][y] == ' ')))
        {  
            if(!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if(board[index_x + j][index_y + j] != ' ')
        {
            break;
        }
    }
    return 1 * (move_count - 14);
}

static inline int index(char piece) 
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

static inline unsigned long long int getEvalHash(char board[8][8], int color)
{
    unsigned long long int h = 0;
    for(int x = 0; x < 8; x++)
    {
        for(int y = 0; y < 8; y++)
        {
            if(board[x][y] != ' ')
            {
                int piece = index(board[x][y]);
                h ^= table[x][y][piece];
            }
        }
    }
    //encode turn
    if(color == 1)
    {
        h ^= turn;
    }

    return h;
}

static inline struct Eval *probeEvalTT(unsigned long long int key)
{
    //get the hash 
    int hashIndex = key % EVALSIZE;
    if(Evaltt[hashIndex].valid == true && Evaltt[hashIndex].key == key)
    {    
        return &Evaltt[hashIndex];
    }
    else
    {
        return NULL;
    }

}

static inline void storeEvalTT(unsigned long long int key, int evaluation)
{
    //get the hash 
    int hashIndex = key % EVALSIZE;

    Evaltt[hashIndex].key = key;
    Evaltt[hashIndex].evaluation = evaluation;
    Evaltt[hashIndex].valid = true;
}

void clearEvalTT(bool print_usage)
{
    float count = 0;
    for(int x = 0; x < EVALSIZE; x++)
    {
        if(Evaltt[x].valid == true)
            count++;
        Evaltt[x].key = 0;
        Evaltt[x].evaluation = 0;
        Evaltt[x].valid = false;
    }

    if(print_usage)
        printf("Table Usage: %.2f%%\n", count * 100 / EVALSIZE);
}

//1: black
//-1: white
int evaluate(char board[8][8], int color)
{
    //tt probe
    unsigned long long int key = getEvalHash(board, color);
    struct Eval* entry = probeEvalTT(key);
    if(entry != NULL)
    { 
        return entry -> evaluation;
    }

    int P_count = 0, R_count = 0, N_count = 0, B_count = 0, Q_count = 0; // White Chess piece counts
    int p_count = 0, r_count = 0, n_count = 0, b_count = 0, q_count = 0; // Black Chess piece counts
    int points = 0;
    int position_bonus_white = 0;
    int position_bonus_black = 0;
    int other_bonus_white = 0;
    int other_bonus_black = 0;
    int white_king_x = -1;
    int white_king_y = -1;
    int black_king_x = -1;
    int black_king_y = -1;
    int wqueen[4] = {-1, -1, -1, -1};
    int bqueen[4] = {-1, -1, -1, -1};
    int wknight[4] = {-1, -1, -1, -1};
    int bknight[4] = {-1, -1, -1, -1};  
    int wrook[4] = {-1, -1, -1, -1};
    int brook[4] = {-1, -1, -1, -1};
    int wbishop[4] = {-1, -1, -1, -1};
    int bbishop[4] = {-1, -1, -1, -1};
    int tempo = 0;   

    for(int x = 0; x < 8; x++)
 	{
		for(int y = 0; y < 8; y++)
		{
            switch(board[x][y])
            {
                case 'P':
                {
                    P_count++;
                    if(passed_white(board, x, y))
                    {
                        position_bonus_white += PawnPassed_white[x];
                    }
                    else
                    {
                        position_bonus_white += white_pawn[x][y];
                    }
                    
                    if(connected_white(board, x, y))
                    {
                        other_bonus_white += CONNECTEDPAWN;    //connected pawn bonus
                    }
                    else if(isolated_white(board, y))
                    {
                        other_bonus_white -= ISOLATEDPAWN;
                    }
                    
                    if(backward_white(board, x, y))
                    {
                        other_bonus_white -= BACKWARDPAWN;                        
                    }                    
                    
                    if(board[x+1][y] == 'P' || board[x-1][y] == 'P')
                    {
                        other_bonus_white -= DOUBLEDPAWN;     //doubled pawn penalty
                    }
                    break;
                }
                case 'R':
                {
                    R_count++;
                    position_bonus_white += white_rook[x][y];
                    switch(R_count)
                    {
                        case 1:
                            wrook[0] = x;
                            wrook[1] = y;
                            break;
                        case 2:
                            wrook[2] = x;
                            wrook[3] = y;
                            break;                        
                    }
                    if(openFile(board, y))
                    {
                        other_bonus_white += ROOKOPENFILE;
                    }
                    else if(semiOpenFile_white(board, y))
                    {
                        other_bonus_white += ROOKHALFFILE;
                    }
                    break;
                }
                case 'N':
                {
                    N_count++;
                    position_bonus_white += white_knight[x][y];
                    switch(N_count)
                    {
                        case 1:
                            wknight[0] = x;
                            wknight[1] = y;
                            break;
                        case 2:
                            wknight[2] = x;
                            wknight[3] = y;
                            break;                        
                    }
                    if(outpost_white(board, x, y))
                    {
                        other_bonus_white += OUTPOST; //outpost bonus
                    }
                    //mobility
                    other_bonus_white += wknight_mobility(board, x, y);
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
                    break;
                }
                case 'B':
                {
                    B_count++;
                    position_bonus_white += white_bishop[x][y];
                    switch(B_count)
                    {
                        case 1:
                            wbishop[0] = x;
                            wbishop[1] = y;
                            break;
                        case 2:
                            wbishop[2] = x;
                            wbishop[3] = y;
                            break;                        
                    }
                    if(badBishop_white(board, x, y))
                    {
                        other_bonus_white -= BADBISHOP;    //bad bishop penalty
                    }
                    //mobility
                    other_bonus_white += wbishop_mobility(board, x, y);
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
                    break;
                }
                case 'Q':
                {
                    Q_count++;
                    position_bonus_white += white_queen[x][y];
                    switch(Q_count)
                    {
                        case 1:
                            wqueen[0] = x;
                            wqueen[1] = y;
                            break;
                        case 2:
                            wqueen[2] = x;
                            wqueen[3] = y;
                            break;                        
                    }
                    //queen early development penalty
                    if(x < 6)
                    {
                        if(board[7][1] == 'N')
                        {
                            other_bonus_white -= 5;
                        }
                        if(board[7][2] == 'B')
                        {
                            other_bonus_white -= 5;
                        }
                        if(board[7][5] == 'B')
                        {
                            other_bonus_white -= 5;
                        }
                        if(board[7][6] == 'N')
                        {
                            other_bonus_white -= 5;
                        }
                    }
                    break;
                }
                case 'K':
                {
                    white_king_x = x;
                    white_king_y = y;
                    break;
                }
                case 'p':
                {
                    p_count++;
                    if(passed_black(board, x, y))
                    {
                        position_bonus_black += PawnPassed_black[x];
                    }
                    else
                    {
                        position_bonus_black += black_pawn[x][y];
                    }
                    
                    if(connected_black(board, x, y))
                    {
                        other_bonus_black += CONNECTEDPAWN;    //connected pawn bonus
                    }
                    else if(isolated_black(board, y))
                    {
                        other_bonus_black -= ISOLATEDPAWN;
                    }

                    if(backward_black(board, x, y))
                    {
                        other_bonus_black -= BACKWARDPAWN;                        
                    }
                    
                    if(board[x-1][y] == 'p' || board[x+1][y] == 'p')
                    {
                        other_bonus_black -= DOUBLEDPAWN;     //doubled pawn penalty
                    }
                    break;
                }
                case 'r':
                {
                    r_count++;
                    position_bonus_black += black_rook[x][y];
                    switch(r_count)
                    {
                        case 1:
                            brook[0] = x;
                            brook[1] = y;
                            break;
                        case 2:
                            brook[2] = x;
                            brook[3] = y;
                            break;                        
                    }
                    if(openFile(board, y))
                    {
                        other_bonus_black += ROOKOPENFILE;
                    }
                    else if(semiOpenFile_black(board, y))
                    {
                        other_bonus_black += ROOKHALFFILE;
                    }
                    break;
                }
                case 'n':
                {
                    n_count++;
                    position_bonus_black += black_knight[x][y];
                    switch(n_count)
                    {
                        case 1:
                            bknight[0] = x;
                            bknight[1] = y;
                            break;
                        case 2:
                            bknight[2] = x;
                            bknight[3] = y;
                            break;                        
                    }
                    if(outpost_black(board, x, y))
                    {
                        other_bonus_black += OUTPOST; //outpost bonus
                    }
                    //mobility
                    other_bonus_black += bknight_mobility(board, x, y);
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
                    break;
                }
                case 'b':
                {
                    b_count++;
                    position_bonus_black += black_bishop[x][y];
                    switch(b_count)
                    {
                        case 1:
                            bbishop[0] = x;
                            bbishop[1] = y;
                            break;
                        case 2:
                            bbishop[2] = x;
                            bbishop[3] = y;
                            break;                        
                    }
                    if(badBishop_black(board, x, y))
                    {
                        other_bonus_black -= BADBISHOP;    //bad bishop penalty
                    }
                    //mobility
                    other_bonus_black += bbishop_mobility(board, x, y);
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
                    break;
                }
                case 'q':
                {
                    q_count++;
                    position_bonus_black += black_queen[x][y];
                    switch(q_count)
                    {
                        case 1:
                            bqueen[0] = x;
                            bqueen[1] = y;
                            break;
                        case 2:
                            bqueen[2] = x;
                            bqueen[3] = y;
                            break;                        
                    }
                    //queen early development penalty
                    if(x > 1)
                    {
                        if(board[0][1] == 'n')
                        {
                            other_bonus_black -= 5;
                        }
                        if(board[0][2] == 'b')
                        {
                            other_bonus_black -= 5;
                        }
                        if(board[0][5] == 'b')
                        {
                            other_bonus_black -= 5;
                        }
                        if(board[0][6] == 'n')
                        {
                            other_bonus_black -= 5;
                        }
                    }
                    break;
                } 
                case 'k':
                {
                    black_king_x = x;
                    black_king_y = y;
                    break;
                } 
                default:
                {
                    continue;
                    break;
                }
            }
        }
    }

    //material draw
    if(!P_count && !p_count)
    {
        if(!R_count && !r_count && !Q_count && !q_count)
        {
            if(!B_count && !b_count)
            {
                if(N_count < 3 && n_count < 3)
                {
                    return 0;
                }
            }
            else if(!N_count && !n_count)
            {
                if(abs(B_count - b_count) < 2)
                {
                    return 0;
                }
            }
            else if((N_count < 3 && !B_count) || (B_count == 1 && !N_count))
            {
                if((n_count < 3 && !b_count) || (b_count == 1 && !n_count))
                {
                    return 0;
                }
            }
        }
        else if(!Q_count && !q_count)
        {
            if(R_count == 1 && r_count == 1)
            {
                if((N_count + B_count) < 2 && (n_count + b_count) < 2)
                {
                    return 0;
                }
            }
            else if(R_count == 1 && !r_count)
            {
                if((N_count + B_count == 0) && (((n_count + b_count) == 1) || ((n_count + b_count) == 2))) 
                { 
                    return 0; 
                }
            }
            else if(r_count == 1 && !R_count)
            {
                if((n_count + b_count == 0) && (((N_count + B_count) == 1) || ((N_count + B_count) == 2))) 
                { 
                    return 0; 
                    
                }
            }
        }
    }

    //for kings' position bonus midgame
    if((R_count * 5 + N_count * 3 + B_count * 3 + Q_count * 9 > 14 || r_count * 5 + n_count * 3 + b_count * 3 + q_count * 9 > 14) && (P_count + p_count + N_count + n_count + B_count + b_count + R_count + r_count + Q_count + q_count) > 12)
    {
        position_bonus_white += white_king_midgame[white_king_x][white_king_y];
        position_bonus_black += black_king_midgame[black_king_x][black_king_y];
        //pawn shield bonus
        if(white_king_x > 5 && white_king_y > 4)
        {
            if(board[7][6] == 'R' || board[7][7] == 'R') //blocked rook penalty
                other_bonus_white -= 40;
            
            if(board[6][5] == 'P')  
                other_bonus_white += 15;
            else if(board[5][5] == 'P')
                other_bonus_white += 10;
            else if(semiOpenFile_white(board, 5)) //semi-open file penalty
                other_bonus_white -= 15;
            
            if(board[5][5] == 'p')  //pawn storm penalty
                other_bonus_white -= 10;
            else if(board[4][5] == 'p')
                other_bonus_white -= 5;
            
            if(board[6][6] == 'P')
                other_bonus_white += 15;
            else if(board[5][6] == 'P')
                other_bonus_white += 10;
            else if(semiOpenFile_white(board, 6))
                other_bonus_white -= 15;

            if(board[5][6] == 'p')
                other_bonus_white -= 10;
            else if(board[4][6] == 'p')
                other_bonus_white -= 5;    
            
            if(board[6][7] == 'P')
                other_bonus_white += 15;
            else if(board[5][7] == 'P')
                other_bonus_white += 10;
            else if(semiOpenFile_white(board, 7))
                other_bonus_white -= 15;
            
            if(board[5][7] == 'p')
                other_bonus_white -= 10;
            else if(board[4][7] == 'p')
                other_bonus_white -= 5;  
        }
        else if(white_king_x > 5 && white_king_y < 3)
        {
            if(board[7][0] == 'R' || board[7][1] == 'R')
                other_bonus_white -= 40;
            
            if(board[6][0] == 'P')
                other_bonus_white += 15;
            else if(board[5][0] == 'P')
                other_bonus_white += 10; 
            else if(semiOpenFile_white(board, 0))
                other_bonus_white -= 15;

            if(board[5][0] == 'p')
                other_bonus_white -= 10;
            else if(board[4][0] == 'p')
                other_bonus_white -= 5;  
            
            if(board[6][1] == 'P')
                other_bonus_white += 15;
            else if(board[5][1] == 'P')
                other_bonus_white += 10;
            else if(semiOpenFile_white(board, 1))
                other_bonus_white -= 15;

            if(board[5][1] == 'p')
                other_bonus_white -= 10;
            else if(board[4][1] == 'p')
                other_bonus_white -= 5;  
            
            if(board[6][2] == 'P')
                other_bonus_white += 15;
            else if(board[5][2] == 'P')
                other_bonus_white += 10;
            else if(semiOpenFile_white(board, 2))
                other_bonus_white -= 15;

            if(board[5][2] == 'p')
                other_bonus_white -= 10;
            else if(board[4][2] == 'p')
                other_bonus_white -= 5;  
        }

        if(black_king_x < 2 && black_king_y > 4)
        {
            if(board[0][6] == 'r' || board[0][7] == 'r')
                other_bonus_black -= 40;
            
            if(board[1][5] == 'p')
                other_bonus_black += 15;
            else if(board[2][5] == 'p')
                other_bonus_black += 10;
            else if(semiOpenFile_black(board, 5))
                other_bonus_black -= 15;

            if(board[2][5] == 'P')
                other_bonus_black -= 10;
            else if(board[3][5] == 'P')
                other_bonus_black -= 5;  
            
            if(board[1][6] == 'p')
                other_bonus_black += 15;
            else if(board[2][6] == 'p')
                other_bonus_black += 10;
            else if(semiOpenFile_black(board, 6))
                other_bonus_black -= 15;
            
            if(board[2][6] == 'P')
                other_bonus_black -= 10;
            else if(board[3][6] == 'P')
                other_bonus_black -= 5; 

            if(board[1][7] == 'p')
                other_bonus_black += 15;
            else if(board[2][7] == 'p')
                other_bonus_black += 10;
            else if(semiOpenFile_black(board, 7))
                other_bonus_black -= 15;
            
            if(board[2][7] == 'P')
                other_bonus_black -= 10;
            else if(board[3][7] == 'P')
                other_bonus_black -= 5; 
        }
        else if(black_king_x < 2 && black_king_y < 3)
        {
            if(board[0][0] == 'r' || board[0][1] == 'r')
                other_bonus_black -= 40;

            if(board[1][0] == 'p')
                other_bonus_black += 15;
            else if(board[2][0] == 'p')
                other_bonus_black += 10;
            else if(semiOpenFile_black(board, 0))
                other_bonus_black -= 15;
            
            if(board[2][0] == 'P')
                other_bonus_black -= 10;
            else if(board[3][0] == 'P')
                other_bonus_black -= 5; 
            
            if(board[1][1] == 'p')
                other_bonus_black += 15;
            else if(board[2][1] == 'p')
                other_bonus_black += 10;
            else if(semiOpenFile_black(board, 1))
                other_bonus_black -= 15;
            
            if(board[2][1] == 'P')
                other_bonus_black -= 10;
            else if(board[3][1] == 'P')
                other_bonus_black -= 5; 
            
            if(board[1][2] == 'p')
                other_bonus_black += 15;
            else if(board[2][2] == 'p')
                other_bonus_black += 10;
            else if(semiOpenFile_black(board, 2))
                other_bonus_black -= 15;
            
            if(board[2][2] == 'P')
                other_bonus_black -= 10;
            else if(board[3][2] == 'P')
                other_bonus_black -= 5; 
        }

        //tempo
        tempo = color * TEMPO;

        //king tropism middlegame
        switch(Q_count)
        {
            case 0:
                break;
            case 1:
                other_bonus_black += (abs(wqueen[0] - black_king_x) + abs(wqueen[1] - black_king_y) - 7)*2; 
                //mobility
                other_bonus_white += wqueen_mobility(board, wqueen[0], wqueen[1]); 
                break;
            default:
                other_bonus_black += (abs(wqueen[0] - black_king_x) + abs(wqueen[1] - black_king_y) + abs(wqueen[2] - black_king_x) + abs(wqueen[3] - black_king_y) - 14)*2; 
                //mobility
                other_bonus_white += (wqueen_mobility(board, wqueen[0], wqueen[1]) + wqueen_mobility(board, wqueen[2], wqueen[3])); 
                break;                 
        }
        switch(q_count)
        {
            case 0:
                break;
            case 1:
                other_bonus_white += (abs(bqueen[0] - white_king_x) + abs(bqueen[1] - white_king_y) - 7)*2; 
                //mobility
                other_bonus_black += bqueen_mobility(board, bqueen[0], bqueen[1]);
                break;
            default:
                other_bonus_white += (abs(bqueen[0] - white_king_x) + abs(bqueen[1] - white_king_y) + abs(bqueen[2] - white_king_x) + abs(bqueen[3] - white_king_y)- 14)*2; 
                //mobility
                other_bonus_black += (bqueen_mobility(board, bqueen[0], bqueen[1]) + bqueen_mobility(board, bqueen[2], bqueen[3]));
                break;                       
        }
        switch(N_count)
        {
            case 0:
                break;
            case 1:
                other_bonus_black += (abs(wknight[0] - black_king_x) + abs(wknight[1] - black_king_y) - 7)*3; 
                break;
            default:
                other_bonus_black += (abs(wknight[0] - black_king_x) + abs(wknight[1] - black_king_y) + abs(wknight[2] - black_king_x) + abs(wknight[3] - black_king_y) - 14)*3; 
                break;                       
        }
        switch(n_count)
        {
            case 0:
                break;
            case 1:
                other_bonus_white += (abs(bknight[0] - white_king_x) + abs(bknight[1] - white_king_y) - 7)*3; 
                break;
            default:
                other_bonus_white += (abs(bknight[0] - white_king_x) + abs(bknight[1] - white_king_y) + abs(bknight[2] - white_king_x) + abs(bknight[3] - white_king_y) - 14)*3; 
                break;                      
        }
        switch(R_count)
        {
            case 0:
                break;
            case 1:
                other_bonus_black += (abs(wrook[0] - black_king_x) + abs(wrook[1] - black_king_y) - 7)*2; 
                //mobility
                other_bonus_white += wrook_mobility(board, wrook[0], wrook[1]);
                break;
            default:
                other_bonus_black += (abs(wrook[0] - black_king_x) + abs(wrook[1] - black_king_y) + abs(wrook[2] - black_king_x) + abs(wrook[3] - black_king_y) - 14)*2; 
                //mobility
                other_bonus_white += (wrook_mobility(board, wrook[0], wrook[1]) + wrook_mobility(board, wrook[2], wrook[3]));
                break;                       
        }
        switch(r_count)
        {
            case 0:
                break;
            case 1:
                other_bonus_white += (abs(brook[0] - white_king_x) + abs(brook[1] - white_king_y) - 7)*2; 
                //mobility
                other_bonus_black += brook_mobility(board, brook[0], brook[1]);
                break;
            default:
                other_bonus_white += (abs(brook[0] - white_king_x) + abs(brook[1] - white_king_y) + abs(brook[2] - white_king_x) + abs(brook[3] - white_king_y) - 14)*2; 
                //mobility
                other_bonus_black += (brook_mobility(board, brook[0], brook[1]) + brook_mobility(board, brook[2], brook[3]));
                break;                       
        }
        switch(B_count)
        {
            case 0:
                break;
            case 1:
                other_bonus_black += (abs(wbishop[0] - black_king_x) + abs(wbishop[1] - black_king_y) - 7)*2; 
                break;
            default:
                other_bonus_black += (abs(wbishop[0] - black_king_x) + abs(wbishop[1] - black_king_y) + abs(wbishop[2] - black_king_x) + abs(wbishop[3] - black_king_y) - 14)*2; 
                break;                       
        }
        switch(b_count)
        {
            case 0:
                break;
            case 1:
                other_bonus_white += (abs(bbishop[0] - white_king_x) + abs(bbishop[1] - white_king_y) - 7)*2; 
                break;
            default:
                other_bonus_white += (abs(bbishop[0] - white_king_x) + abs(bbishop[1] - white_king_y) + abs(bbishop[2] - white_king_x) + abs(bbishop[3] - white_king_y) - 14)*2; 
                break;                        
        }
    }
    else
    {
        //king position bonus endgame
        position_bonus_white += white_king_endgame[white_king_x][white_king_y];
        position_bonus_black += black_king_endgame[black_king_x][black_king_y];

        //king tropism endgame
        switch(Q_count)
        {
            case 0:
                break;
            case 1:
                other_bonus_black += (abs(wqueen[0] - black_king_x) + abs(wqueen[1] - black_king_y) - 7)*4; 
                //mobility
                other_bonus_white += wqueen_mobility(board, wqueen[0], wqueen[1])*2; 
                break;
            default:
                other_bonus_black += (abs(wqueen[0] - black_king_x) + abs(wqueen[1] - black_king_y) + abs(wqueen[2] - black_king_x) + abs(wqueen[3] - black_king_y) - 14)*4; 
                //mobility
                other_bonus_white += (wqueen_mobility(board, wqueen[0], wqueen[1]) + wqueen_mobility(board, wqueen[2], wqueen[3]))*2; 
                break;                       
        }
        switch(q_count)
        {
            case 0:
                break;
            case 1:
                other_bonus_white += (abs(bqueen[0] - white_king_x) + abs(bqueen[1] - white_king_y) - 7)*4; 
                //mobility
                other_bonus_black += bqueen_mobility(board, bqueen[0], bqueen[1])*2;
                break;
            default:
                other_bonus_white += (abs(bqueen[0] - white_king_x) + abs(bqueen[1] - white_king_y) + abs(bqueen[2] - white_king_x) + abs(bqueen[3] - white_king_y)- 14)*4; 
                //mobility
                other_bonus_black += (bqueen_mobility(board, bqueen[0], bqueen[1]) + bqueen_mobility(board, bqueen[2], bqueen[3]))*2;
                break;                     
        }
        switch(N_count)
        {
            case 0:
                break;
            case 1:
                other_bonus_black += (abs(wknight[0] - black_king_x) + abs(wknight[1] - black_king_y) - 7)*3; 
                break;
            default:
                other_bonus_black += (abs(wknight[0] - black_king_x) + abs(wknight[1] - black_king_y) + abs(wknight[2] - black_king_x) + abs(wknight[3] - black_king_y) - 14)*3; 
                break;                      
        }
        switch(n_count)
        {
            case 0:
                break;
            case 1:
                other_bonus_white += (abs(bknight[0] - white_king_x) + abs(bknight[1] - white_king_y) - 7)*3; 
                break;
            default:
                other_bonus_white += (abs(bknight[0] - white_king_x) + abs(bknight[1] - white_king_y) + abs(bknight[2] - white_king_x) + abs(bknight[3] - white_king_y) - 14)*3; 
                break;                     
        }
        switch(R_count)
        {
            case 0:
                break;
            case 1:
                other_bonus_black += (abs(wrook[0] - black_king_x) + abs(wrook[1] - black_king_y) - 7)*1; 
                //mobility
                other_bonus_white += wrook_mobility(board, wrook[0], wrook[1])*2;
                break;
            default:
                other_bonus_black += (abs(wrook[0] - black_king_x) + abs(wrook[1] - black_king_y) + abs(wrook[2] - black_king_x) + abs(wrook[3] - black_king_y) - 14)*1; 
                //mobility
                other_bonus_white += (wrook_mobility(board, wrook[0], wrook[1]) + wrook_mobility(board, wrook[2], wrook[3]))*2;
                break;                      
        }
        switch(r_count)
        {
            case 0:
                break;
            case 1:
                other_bonus_white += (abs(brook[0] - white_king_x) + abs(brook[1] - white_king_y) - 7)*1;
                //mobility
                other_bonus_black += brook_mobility(board, brook[0], brook[1])*2; 
                break;
            default:
                other_bonus_white += (abs(brook[0] - white_king_x) + abs(brook[1] - white_king_y) + abs(brook[2] - white_king_x) + abs(brook[3] - white_king_y) - 14)*1; 
                //mobility
                other_bonus_black += (brook_mobility(board, brook[0], brook[1]) + brook_mobility(board, brook[2], brook[3]))*2;
                break;                     
        }
        switch(B_count)
        {
            case 0:
                break;
            case 1:
                other_bonus_black += (abs(wbishop[0] - black_king_x) + abs(wbishop[1] - black_king_y) - 7)*1; 
                break;
            default:
                other_bonus_black += (abs(wbishop[0] - black_king_x) + abs(wbishop[1] - black_king_y) + abs(wbishop[2] - black_king_x) + abs(wbishop[3] - black_king_y) - 14)*1; 
                break;                       
        }
        switch(b_count)
        {
            case 0:
                break;
            case 1:
                other_bonus_white += (abs(bbishop[0] - white_king_x) + abs(bbishop[1] - white_king_y) - 7)*1; 
                break;
            default:
                other_bonus_white += (abs(bbishop[0] - white_king_x) + abs(bbishop[1] - white_king_y) + abs(bbishop[2] - white_king_x) + abs(bbishop[3] - white_king_y) - 14)*1; 
                break;                      
        }
    }
    
    points = p_count * 100 + r_count * (500 + rook_val[p_count]) + n_count * (320 + knight_val[p_count]) + b_count * 330 + q_count * 900  + position_bonus_black + ((b_count == 2)? 1 : 0) * BISHOPPAIR + ((n_count == 2)? 1 : 0) * KNIGHTPAIR + ((r_count == 2)? 1 : 0) * ROOKPAIR + other_bonus_black - P_count * 100 - R_count * (500 + rook_val[P_count]) - N_count * (320 + knight_val[P_count]) - B_count * 330 - Q_count * 900 - position_bonus_white - ((B_count == 2)? 1 : 0) * BISHOPPAIR - ((N_count == 2)? 1 : 0) * KNIGHTPAIR - ((R_count == 2)? 1 : 0) * ROOKPAIR - other_bonus_white + tempo; 
    
    //tt store
    storeEvalTT(key, color * points);
    return color * points;
}
