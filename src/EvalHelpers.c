#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>
#include "EvalHelpers.h"
#include "Board.h"

//return how many white pawns are attacking this square
inline int wpawn_controlled(char board[8][8], int x, int y)
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

//return how many black pawns are attacking this square
inline int bpawn_controlled(char board[8][8], int x, int y)
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

//count as candidate passed pawn if no enemy pawn is in the path
//and no square in the path is controlled by more enemy pawns than own pawns
//don't count as passed when there's a doubled pawn in the path
inline bool candidate_passed_white(char board[8][8], int x, int y)
{
    for(int rank = 1; rank < x; rank++)
    {
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

inline bool candidate_passed_black(char board[8][8], int x, int y)
{
    for(int rank = 6; rank > x; rank--)
    {
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

//check if this pawn is supported by own pawns
inline bool connected_white(char board[8][8], int x, int y)
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

inline bool connected_black(char board[8][8], int x, int y)
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

//check if a friendly pawn is next to this pawn
inline bool phalanx_white(char board[8][8], int x, int y)
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

inline bool phalanx_black(char board[8][8], int x, int y)
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

inline bool isolated_white(char board[8][8], int y)
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

inline bool isolated_black(char board[8][8], int y)
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

inline bool backward_white(char board[8][8], int x, int y)
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

inline bool backward_black(char board[8][8], int x, int y)
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

inline bool openFile(char board[8][8], int y)
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

inline bool queenFile(BOARD *pos, int y)
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

inline bool semiOpenFile_white(char board[8][8], int y)
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

inline bool semiOpenFile_black(char board[8][8], int y)
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

//only on rank 4, 5, 6 for white, 5, 4, 3 for black
inline bool outpost_white(char board[8][8], int x, int y)
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

inline bool outpost_black(char board[8][8], int x, int y)
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

//bad bishop when its forward mobility is blocked by own pawn
inline bool badBishop_white(char board[8][8], int x, int y)
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

inline bool badBishop_black(char board[8][8], int x, int y)
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

//penalty for semi-open files around king
inline int wking_file(char board[8][8], int y)
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

inline int bking_file(char board[8][8], int y)
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