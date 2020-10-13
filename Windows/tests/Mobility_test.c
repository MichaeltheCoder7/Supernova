#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>

/*
black pieces
00 01 02 03 04 05 06 07
10 11 12 13 14 15 16 17
20 21 22 23 24 25 26 27
30 31 32 33 34 35 36 37
40 41 42 43 44 45 46 47
50 51 52 53 54 55 56 57
60 61 62 63 64 65 66 67
70 71 72 73 74 75 76 77
white pieces
*/

char board[8][8] = {
						
			{'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
			{'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
			{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},			
			{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
			{' ', ' ', 'q', ' ', ' ', ' ', ' ', ' '},
			{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
			{'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
			{'R', ' ', ' ', 'Q', 'K', 'B', 'N', 'R'},

			};


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
        if(board[x][y] != ' ')
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
        if(board[x][y] != ' ')
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
        if(board[x][y] != ' ')
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
        if(board[x][y] != ' ')
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
        if(board[x][y] != ' ')
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
        if(board[x][y] != ' ')
        {
            break;
        }
    }
    return 1 * (move_count - 14);
}

void main()
{
    int mob = bqueen_mobility(board, 4, 2);
    printf("mob: %d\n", mob);
}