#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>
#include "Mobility.h"
#include "MoveGen.h"

//check if this square is defended by any pawns
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
//including king ring and king's position
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
//exclude space protected by enemy pawns for minor pieces
//exclude own queen for minor pieces
//exclude own blocked pawns and own pawns on rank 2 and 3
//exclude own king
//also consider attack on enemy king
int wknight_mobility(char board[8][8], int index_x, int index_y, int bkingx, int bkingy)
{
    int attack = 0;
    int move_count = 0;
    int x, y;

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

int bknight_mobility(char board[8][8], int index_x, int index_y, int wkingx, int wkingy)
{
    int attack = 0;
    int move_count = 0;
    int x, y;

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

int wbishop_mobility(char board[8][8], int index_x, int index_y, int bkingx, int bkingy)
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

int bbishop_mobility(char board[8][8], int index_x, int index_y, int wkingx, int wkingy)
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

int wrook_mobility(char board[8][8], int index_x, int index_y, int bkingx, int bkingy)
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

int brook_mobility(char board[8][8], int index_x, int index_y, int wkingx, int wkingy)
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

int wqueen_mobility(char board[8][8], int index_x, int index_y, int bkingx, int bkingy)
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

int bqueen_mobility(char board[8][8], int index_x, int index_y, int wkingx, int wkingy)
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