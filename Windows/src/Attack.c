#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>
#include "Attack.h"
#include "Board.h"


//check if check 
//return 1 if yes
//return 0 if no
//color = 1: check if black king is in check
//color = -1: check if white king is in check
int ifCheck(char board[8][8], int color)
{
    int king_x = -1;
    int king_y = -1;

    switch(color)
    {
        case 1:
        {
            for(int x = 0; x < 8; x++)
            {
                for (int y = 7; y >= 0; y--)
                {
                    if(board[x][y] == 'k')
                    {
                        king_x = x;
                        king_y = y;
                        x = 8; //to break out of nested for loops
                        break;
                    }

                }
            }
            //assert(king_x >= 0);
            //down left
            for(int i = 1; i <= 7; i++)
            {
                if(king_x + i > 7 || king_y - i < 0)
                {
                    break;
                }
                if(board[king_x + i][king_y - i] != ' ')
                {
                    if(i == 1 && (board[king_x + i][king_y - i] == 'P' || board[king_x + i][king_y - i] == 'K'))
                    {
                        return 1;
                    }
                    if(board[king_x + i][king_y - i] == 'B' || board[king_x + i][king_y - i] == 'Q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //down right
            for(int i = 1; i <= 7; i++)
            {
                if(king_x + i > 7 || king_y + i > 7)
                {
                    break;
                }
                if(board[king_x + i][king_y + i] != ' ')
                {
                    if(i == 1 && (board[king_x + i][king_y + i] == 'P' || board[king_x + i][king_y + i] == 'K'))
                    {
                        return 1;
                    }
                    if(board[king_x + i][king_y + i] == 'B' || board[king_x + i][king_y + i] == 'Q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //down
            for(int i = king_x+1; i <= 7; i++)
            {
                if(board[i][king_y] != ' ')
                {
                    if(i == king_x+1 && board[i][king_y] == 'K')
                    {
                        return 1;
                    }
                    if(board[i][king_y] == 'R' || board[i][king_y] == 'Q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //left
            for(int i = king_y-1; i >= 0; i--)
            {
                if(board[king_x][i] != ' ')
                {
                    if(i == king_y-1 && board[king_x][i] == 'K')
                    {
                        return 1;
                    }
                    if(board[king_x][i] == 'R' || board[king_x][i] == 'Q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //right
            for(int i = king_y+1; i <= 7; i++)
            {
                if(board[king_x][i] != ' ')
                {
                    if(i == king_y+1 && board[king_x][i] == 'K')
                    {
                        return 1;
                    }
                    if(board[king_x][i] == 'R' || board[king_x][i] == 'Q')
                    {
                        return 1;
                    }
                    break;
                }
            }     
            //knight attacks
            if(king_x + 2 <= 7 && king_y + 1 <= 7)
            {
                if(board[king_x + 2][king_y + 1] == 'N')
                    return 1;
            }
            if(king_x + 2 <= 7 && king_y - 1 >= 0)
            {
                if(board[king_x + 2][king_y - 1] == 'N')
                    return 1;
            }
            if(king_x + 1 <= 7 && king_y + 2 <= 7)
            {
                if(board[king_x + 1][king_y + 2] == 'N')
                    return 1;
            }
            if(king_x + 1 <= 7 && king_y - 2 >= 0)
            {
                if(board[king_x + 1][king_y - 2] == 'N')
                    return 1;
            }
            if(king_x - 2 >= 0 && king_y + 1 <= 7)
            {
                if(board[king_x - 2][king_y + 1] == 'N')
                    return 1;
            }
            if(king_x - 2 >= 0 && king_y - 1 >= 0)
            {
                if(board[king_x - 2][king_y - 1] == 'N')
                    return 1;
            }
            if(king_x - 1 >= 0 && king_y + 2 <= 7)
            {
                if(board[king_x - 1][king_y + 2] == 'N')
                    return 1;
            }
            if(king_x - 1 >= 0 && king_y - 2 >= 0)
            {
                if(board[king_x - 1][king_y - 2] == 'N')
                    return 1;
            }
            //up
            for(int i = king_x-1; i >= 0; i--)
            {
                if(board[i][king_y] != ' ')
                {
                    if(i == king_x-1 && board[i][king_y] == 'K')
                    {
                        return 1;
                    }
                    if(board[i][king_y] == 'R' || board[i][king_y] == 'Q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //up left
            for(int i = 1; i <= 7; i++)
            {
                if(king_x - i < 0 || king_y - i < 0)
                {
                    break;
                }
                if(board[king_x - i][king_y - i] != ' ')
                {
                    if(i == 1 && board[king_x - i][king_y - i] == 'K')
                    {
                        return 1;
                    }
                    if(board[king_x - i][king_y - i] == 'B' || board[king_x - i][king_y - i] == 'Q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //up right
            for(int i = 1; i <= 7; i++)
            {
                if(king_x - i < 0 || king_y + i > 7)
                {
                    break;
                }
                if(board[king_x - i][king_y + i] != ' ')
                {
                    if(i == 1 && board[king_x - i][king_y + i] == 'K')
                    {
                        return 1;
                    }
                    if(board[king_x - i][king_y + i] == 'B' || board[king_x - i][king_y + i] == 'Q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            
            break;
        }
        case -1:
        {
            for(int x = 7; x >= 0; x--)
            {
                for (int y = 7; y >= 0; y--)
                {
                    if(board[x][y] == 'K')
                    {
                        king_x = x;
                        king_y = y;
                        x = -1; //to break out of nested for loops
                        break;
                    }

                }
            }
            //assert(king_x >= 0);
            //up left
            for(int i = 1; i <= 7; i++)
            {
                if(king_x - i < 0 || king_y - i < 0)
                {
                    break;
                }
                if(board[king_x - i][king_y - i] != ' ')
                {
                    if(i == 1 && (board[king_x - i][king_y - i] == 'p' || board[king_x - i][king_y - i] == 'k'))
                    {
                        return 1;
                    }
                    if(board[king_x - i][king_y - i] == 'b' || board[king_x - i][king_y - i] == 'q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //up right
            for(int i = 1; i <= 7; i++)
            {
                if(king_x - i < 0 || king_y + i > 7)
                {
                    break;
                }
                if(board[king_x - i][king_y + i] != ' ')
                {
                    if(i == 1 && (board[king_x - i][king_y + i] == 'p' || board[king_x - i][king_y + i] == 'k'))
                    {
                        return 1;
                    }
                    if(board[king_x - i][king_y + i] == 'b' || board[king_x - i][king_y + i] == 'q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //up
            for(int i = king_x-1; i >= 0; i--)
            {
                if(board[i][king_y] != ' ')
                {
                    if(i == king_x-1 && board[i][king_y] == 'k')
                    {
                        return 1;
                    }
                    if(board[i][king_y] == 'r' || board[i][king_y] == 'q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //left
            for(int i = king_y-1; i >= 0; i--)
            {
                if(board[king_x][i] != ' ')
                {
                    if(i == king_y-1 && board[king_x][i] == 'k')
                    {
                        return 1;
                    }
                    if(board[king_x][i] == 'r' || board[king_x][i] == 'q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //right
            for(int i = king_y+1; i <= 7; i++)
            {
                if(board[king_x][i] != ' ')
                {
                    if(i == king_y+1 && board[king_x][i] == 'k')
                    {
                        return 1;
                    }
                    if(board[king_x][i] == 'r' || board[king_x][i] == 'q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //knight attacks
            if(king_x - 2 >= 0 && king_y + 1 <= 7)
            {
                if(board[king_x - 2][king_y + 1] == 'n')
                    return 1;
            }
            if(king_x - 2 >= 0 && king_y - 1 >= 0)
            {
                if(board[king_x - 2][king_y - 1] == 'n')
                    return 1;
            }
            if(king_x - 1 >= 0 && king_y + 2 <= 7)
            {
                if(board[king_x - 1][king_y + 2] == 'n')
                    return 1;
            }
            if(king_x - 1 >= 0 && king_y - 2 >= 0)
            {
                if(board[king_x - 1][king_y - 2] == 'n')
                    return 1;
            }
            if(king_x + 2 <= 7 && king_y + 1 <= 7)
            {
                if(board[king_x + 2][king_y + 1] == 'n')
                    return 1;
            }
            if(king_x + 2 <= 7 && king_y - 1 >= 0)
            {
                if(board[king_x + 2][king_y - 1] == 'n')
                    return 1;
            }
            if(king_x + 1 <= 7 && king_y + 2 <= 7)
            {
                if(board[king_x + 1][king_y + 2] == 'n')
                    return 1;
            }
            if(king_x + 1 <= 7 && king_y - 2 >= 0)
            {
                if(board[king_x + 1][king_y - 2] == 'n')
                    return 1;
            }
            //down
            for(int i = king_x+1; i <= 7; i++)
            {
                if(board[i][king_y] != ' ')
                {
                    if(i == king_x+1 && board[i][king_y] == 'k')
                    {
                        return 1;
                    }
                    if(board[i][king_y] == 'r' || board[i][king_y] == 'q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //down left
            for(int i = 1; i <= 7; i++)
            {
                if(king_x + i > 7 || king_y - i < 0)
                {
                    break;
                }
                if(board[king_x + i][king_y - i] != ' ')
                {
                    if(i == 1 && board[king_x + i][king_y - i] == 'k')
                    {
                        return 1;
                    }
                    if(board[king_x + i][king_y - i] == 'b' || board[king_x + i][king_y - i] == 'q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //down right
            for(int i = 1; i <= 7; i++)
            {
                if(king_x + i > 7 || king_y + i > 7)
                {
                    break;
                }
                if(board[king_x + i][king_y + i] != ' ')
                {
                    if(i == 1 && board[king_x + i][king_y + i] == 'k')
                    {
                        return 1;
                    }
                    if(board[king_x + i][king_y + i] == 'b' || board[king_x + i][king_y + i] == 'q')
                    {
                        return 1;
                    }
                    break;
                }
            }

            break;
        }
    }
    return 0;
}


//check if a piece in a given position is threatened
//return 1 if yes
//return 0 if no
//color = 1: check if a position is threatened by white pieces
//color = -1: check if a position is threatened by black pieces
int isThreatened(char board[8][8], char location[3], int color)
{
    int x = position_to_x(location);
    int y = position_to_y(location);

    switch(color)
    {
        case 1:
        {
            //left
            for(int i = y-1; i >= 0; i--)
            {
                if(board[x][i] != ' ')
                {
                    if(i == y-1 && board[x][i] == 'K')
                    {
                        return 1;
                    }
                    if(board[x][i] == 'R' || board[x][i] == 'Q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //right
            for(int i = y+1; i <= 7; i++)
            {
                if(board[x][i] != ' ')
                {
                    if(i == y+1 && board[x][i] == 'K')
                    {
                        return 1;
                    }
                    if(board[x][i] == 'R' || board[x][i] == 'Q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //up
            for(int i = x-1; i >= 0; i--)
            {
                if(board[i][y] != ' ')
                {
                    if(i == x-1 && board[i][y] == 'K')
                    {
                        return 1;
                    }
                    if(board[i][y] == 'R' || board[i][y] == 'Q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //down
            for(int i = x+1; i <= 7; i++)
            {
                if(board[i][y] != ' ')
                {
                    if(i == x+1 && board[i][y] == 'K')
                    {
                        return 1;
                    }
                    if(board[i][y] == 'R' || board[i][y] == 'Q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //down left
            for(int i = 1; i <= 7; i++)
            {
                if(x + i > 7 || y - i < 0)
                {
                    break;
                }
                if(board[x + i][y - i] != ' ')
                {
                    if(i == 1 && (board[x + i][y - i] == 'P' || board[x + i][y - i] == 'K'))
                    {
                        return 1;
                    }
                    if(board[x + i][y - i] == 'B' || board[x + i][y - i] == 'Q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //down right
            for(int i = 1; i <= 7; i++)
            {
                if(x + i > 7 || y + i > 7)
                {
                    break;
                }
                if(board[x + i][y + i] != ' ')
                {
                    if(i == 1 && (board[x + i][y + i] == 'P' || board[x + i][y + i] == 'K'))
                    {
                        return 1;
                    }
                    if(board[x + i][y + i] == 'B' || board[x + i][y + i] == 'Q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //up left
            for(int i = 1; i <= 7; i++)
            {
                if(x - i < 0 || y - i < 0)
                {
                    break;
                }
                if(board[x - i][y - i] != ' ')
                {
                    if(i == 1 && board[x - i][y - i] == 'K')
                    {
                        return 1;
                    }
                    if(board[x - i][y - i] == 'B' || board[x - i][y - i] == 'Q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //up right
            for(int i = 1; i <= 7; i++)
            {
                if(x - i < 0 || y + i > 7)
                {
                    break;
                }
                if(board[x - i][y + i] != ' ')
                {
                    if(i == 1 && board[x - i][y + i] == 'K')
                    {
                        return 1;
                    }
                    if(board[x - i][y + i] == 'B' || board[x - i][y + i] == 'Q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //knight attacks
            if(x - 2 >= 0 && y + 1 <= 7)
            {
                if(board[x - 2][y + 1] == 'N')
                    return 1;
            }
            if(x - 2 >= 0 && y - 1 >= 0)
            {
                if(board[x - 2][y - 1] == 'N')
                    return 1;
            }
            if(x - 1 >= 0 && y + 2 <= 7)
            {
                if(board[x - 1][y + 2] == 'N')
                    return 1;
            }
            if(x - 1 >= 0 && y - 2 >= 0)
            {
                if(board[x - 1][y - 2] == 'N')
                    return 1;
            }
            if(x + 2 <= 7 && y + 1 <= 7)
            {
                if(board[x + 2][y + 1] == 'N')
                    return 1;
            }
            if(x + 2 <= 7 && y - 1 >= 0)
            {
                if(board[x + 2][y - 1] == 'N')
                    return 1;
            }
            if(x + 1 <= 7 && y + 2 <= 7)
            {
                if(board[x + 1][y + 2] == 'N')
                    return 1;
            }
            if(x + 1 <= 7 && y - 2 >= 0)
            {
                if(board[x + 1][y - 2] == 'N')
                    return 1;
            }
            break;
        }
        
        case -1:
        {
            //left
            for(int i = y-1; i >= 0; i--)
            {
                if(board[x][i] != ' ')
                {
                    if(i == y-1 && board[x][i] == 'k')
                    {
                        return 1;
                    }
                    if(board[x][i] == 'r' || board[x][i] == 'q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //right
            for(int i = y+1; i <= 7; i++)
            {
                if(board[x][i] != ' ')
                {
                    if(i == y+1 && board[x][i] == 'k')
                    {
                        return 1;
                    }
                    if(board[x][i] == 'r' || board[x][i] == 'q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //up
            for(int i = x-1; i >= 0; i--)
            {
                if(board[i][y] != ' ')
                {
                    if(i == x-1 && board[i][y] == 'k')
                    {
                        return 1;
                    }
                    if(board[i][y] == 'r' || board[i][y] == 'q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //down
            for(int i = x+1; i <= 7; i++)
            {
                if(board[i][y] != ' ')
                {
                    if(i == x+1 && board[i][y] == 'k')
                    {
                        return 1;
                    }
                    if(board[i][y] == 'r' || board[i][y] == 'q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //down left
            for(int i = 1; i <= 7; i++)
            {
                if(x + i > 7 || y - i < 0)
                {
                    break;
                }
                if(board[x + i][y - i] != ' ')
                {
                    if(i == 1 && board[x + i][y - i] == 'k')
                    {
                        return 1;
                    }
                    if(board[x + i][y - i] == 'b' || board[x + i][y - i] == 'q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //down right
            for(int i = 1; i <= 7; i++)
            {
                if(x + i > 7 || y + i > 7)
                {
                    break;
                }
                if(board[x + i][y + i] != ' ')
                {
                    if(i == 1 && board[x + i][y + i] == 'k')
                    {
                        return 1;
                    }
                    if(board[x + i][y + i] == 'b' || board[x + i][y + i] == 'q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //up left
            for(int i = 1; i <= 7; i++)
            {
                if(x - i < 0 || y - i < 0)
                {
                    break;
                }
                if(board[x - i][y - i] != ' ')
                {
                    if(i == 1 && (board[x - i][y - i] == 'p' || board[x - i][y - i] == 'k'))
                    {
                        return 1;
                    }
                    if(board[x - i][y - i] == 'b' || board[x - i][y - i] == 'q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //up right
            for(int i = 1; i <= 7; i++)
            {
                if(x - i < 0 || y + i > 7)
                {
                    break;
                }
                if(board[x - i][y + i] != ' ')
                {
                    if(i == 1 && (board[x - i][y + i] == 'p' || board[x - i][y + i] == 'k'))
                    {
                        return 1;
                    }
                    if(board[x - i][y + i] == 'b' || board[x - i][y + i] == 'q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //knight attacks
            if(x - 2 >= 0 && y + 1 <= 7)
            {
                if(board[x - 2][y + 1] == 'n')
                    return 1;
            }
            if(x - 2 >= 0 && y - 1 >= 0)
            {
                if(board[x - 2][y - 1] == 'n')
                    return 1;
            }
            if(x - 1 >= 0 && y + 2 <= 7)
            {
                if(board[x - 1][y + 2] == 'n')
                    return 1;
            }
            if(x - 1 >= 0 && y - 2 >= 0)
            {
                if(board[x - 1][y - 2] == 'n')
                    return 1;
            }
            if(x + 2 <= 7 && y + 1 <= 7)
            {
                if(board[x + 2][y + 1] == 'n')
                    return 1;
            }
            if(x + 2 <= 7 && y - 1 >= 0)
            {
                if(board[x + 2][y - 1] == 'n')
                    return 1;
            }
            if(x + 1 <= 7 && y + 2 <= 7)
            {
                if(board[x + 1][y + 2] == 'n')
                    return 1;
            }
            if(x + 1 <= 7 && y - 2 >= 0)
            {
                if(board[x + 1][y - 2] == 'n')
                    return 1;
            }
            break;
        }
    }
    
    return 0;
}

//1: white attackers
//-1: black attackers
//return index 0 - 63 of the smallest attacker on the board
//return -1 if not attacked
int get_smallest_attacker(char board[8][8], char location[3], int color)
{
    int x = position_to_x(location);
    int y = position_to_y(location);
    int attackers_index[2] = {-2, -2}; //for queen and king

    switch(color)
    {
        case 1:
        {
            //pawn attack
            if(x + 1 <= 7 && y - 1 >= 0)
            {
                if(board[x + 1][y - 1] == 'P')
                {
                    return 8*(x+1) + (y-1);
                }
            }
            if(x + 1 <= 7 && y + 1 <= 7)
            {
                if(board[x + 1][y + 1] == 'P')
                {
                    return 8*(x+1) + (y+1);
                }
            }
            //knight attacks
            if(x + 2 <= 7 && y + 1 <= 7)
            {
                if(board[x + 2][y + 1] == 'N')
                    return 8*(x+2) + (y+1);
            }
            if(x + 2 <= 7 && y - 1 >= 0)
            {
                if(board[x + 2][y - 1] == 'N')
                    return 8*(x+2) + (y-1);
            }
            if(x + 1 <= 7 && y + 2 <= 7)
            {
                if(board[x + 1][y + 2] == 'N')
                    return 8*(x+1) + (y+2);
            }
            if(x + 1 <= 7 && y - 2 >= 0)
            {
                if(board[x + 1][y - 2] == 'N')
                    return 8*(x+1) + (y-2);
            }
            if(x - 2 >= 0 && y + 1 <= 7)
            {
                if(board[x - 2][y + 1] == 'N')
                    return 8*(x-2) + (y+1);
            }
            if(x - 2 >= 0 && y - 1 >= 0)
            {
                if(board[x - 2][y - 1] == 'N')
                    return 8*(x-2) + (y-1);
            }
            if(x - 1 >= 0 && y + 2 <= 7)
            {
                if(board[x - 1][y + 2] == 'N')
                    return 8*(x-1) + (y+2);
            }
            if(x - 1 >= 0 && y - 2 >= 0)
            {
                if(board[x - 1][y - 2] == 'N')
                    return 8*(x-1) + (y-2);
            }
            //down left
            for(int i = 1; i <= 7; i++)
            {
                if(x + i > 7 || y - i < 0)
                {
                    break;
                }
                if(board[x + i][y - i] != ' ')
                {
                    if(board[x + i][y - i] == 'B')
                    {
                        return 8*(x+i) + (y-i);
                    }
                    else if(board[x + i][y - i] == 'Q')
                    {
                        attackers_index[0] = 8*(x+i) + (y-i);
                    }
                    else if(i == 1 && board[x + i][y - i] == 'K')
                    {
                        attackers_index[1] = 8*(x+i) + (y-i);
                    }
                    break;
                }
            }
            //down right
            for(int i = 1; i <= 7; i++)
            {
                if(x + i > 7 || y + i > 7)
                {
                    break;
                }
                if(board[x + i][y + i] != ' ')
                {
                    if(board[x + i][y + i] == 'B')
                    {
                        return 8*(x+i) + (y+i);
                    }
                    else if(board[x + i][y + i] == 'Q')
                    {
                        attackers_index[0] = 8*(x+i) + (y+i);
                    }
                    else if(i == 1 && board[x + i][y + i] == 'K')
                    {
                        attackers_index[1] = 8*(x+i) + (y+i);
                    }
                    break;
                }
            }
            //up left
            for(int i = 1; i <= 7; i++)
            {
                if(x - i < 0 || y - i < 0)
                {
                    break;
                }
                if(board[x - i][y - i] != ' ')
                {
                    if(board[x - i][y - i] == 'B')
                    {
                        return 8*(x-i) + (y-i);
                    }
                    else if(board[x - i][y - i] == 'Q')
                    {
                        attackers_index[0] = 8*(x-i) + (y-i);
                    }
                    else if(i == 1 && board[x - i][y - i] == 'K')
                    {
                        attackers_index[1] = 8*(x-i) + (y-i);
                    }
                    break;
                }
            }
            //up right
            for(int i = 1; i <= 7; i++)
            {
                if(x - i < 0 || y + i > 7)
                {
                    break;
                }
                if(board[x - i][y + i] != ' ')
                {
                    if(board[x - i][y + i] == 'B')
                    {
                        return 8*(x-i) + (y+i);
                    }
                    else if(board[x - i][y + i] == 'Q')
                    {
                        attackers_index[0] = 8*(x-i) + (y+i);
                    }
                    else if(i == 1 && board[x - i][y + i] == 'K')
                    {
                        attackers_index[1] = 8*(x-i) + (y+i);
                    }
                    break;
                }
            }
            //down
            for(int i = x+1; i <= 7; i++)
            {
                if(board[i][y] != ' ')
                {
                    if(board[i][y] == 'R')
                    {
                        return 8*i + y;
                    }
                    else if(board[i][y] == 'Q')
                    {
                        attackers_index[0] = 8*i + y;
                    }
                    else if(i == x+1 && board[i][y] == 'K')
                    {
                        attackers_index[1] = 8*i + y;
                    }
                    break;
                }
            }
            //left
            for(int i = y-1; i >= 0; i--)
            {
                if(board[x][i] != ' ')
                {
                    if(board[x][i] == 'R')
                    {
                        return 8*x + i;
                    }
                    else if(board[x][i] == 'Q')
                    {
                        attackers_index[0] = 8*x + i;
                    }
                    else if(i == y-1 && board[x][i] == 'K')
                    {
                        attackers_index[1] = 8*x + i;
                    }

                    break;
                }
            }
            //right
            for(int i = y+1; i <= 7; i++)
            {
                if(board[x][i] != ' ')
                {
                    if(board[x][i] == 'R')
                    {
                        return 8*x + i;
                    }
                    else if(board[x][i] == 'Q')
                    {
                        attackers_index[0] = 8*x + i;
                    }
                    else if(i == y+1 && board[x][i] == 'K')
                    {
                        attackers_index[1] = 8*x + i;
                    }
                    break;
                }
            }
            //up
            for(int i = x-1; i >= 0; i--)
            {
                if(board[i][y] != ' ')
                {
                    if(board[i][y] == 'R')
                    {
                        return 8*i + y;
                    }
                    else if(board[i][y] == 'Q')
                    {
                        attackers_index[0] = 8*i + y;
                    }
                    else if(i == x-1 && board[i][y] == 'K')
                    {
                        attackers_index[1] = 8*i + y;
                    }
                    break;
                }
            }

            break;
        }
        
        case -1:
        {
            //pawn attack
            if(x - 1 >= 0 && y - 1 >= 0)
            {
                if(board[x - 1][y - 1] == 'p')
                {
                    return 8*(x-1) + (y-1);
                }
            }
            if(x - 1 >= 0 && y + 1 <= 7)
            {
                if(board[x - 1][y + 1] == 'p')
                {
                    return 8*(x-1) + (y+1);
                }
            }
            //knight attacks
            if(x - 2 >= 0 && y + 1 <= 7)
            {
                if(board[x - 2][y + 1] == 'n')
                    return 8*(x-2) + (y+1);
            }
            if(x - 2 >= 0 && y - 1 >= 0)
            {
                if(board[x - 2][y - 1] == 'n')
                    return 8*(x-2) + (y-1);
            }
            if(x - 1 >= 0 && y + 2 <= 7)
            {
                if(board[x - 1][y + 2] == 'n')
                    return 8*(x-1) + (y+2);
            }
            if(x - 1 >= 0 && y - 2 >= 0)
            {
                if(board[x - 1][y - 2] == 'n')
                    return 8*(x-1) + (y-2);
            }
            if(x + 2 <= 7 && y + 1 <= 7)
            {
                if(board[x + 2][y + 1] == 'n')
                    return 8*(x+2) + (y+1);
            }
            if(x + 2 <= 7 && y - 1 >= 0)
            {
                if(board[x + 2][y - 1] == 'n')
                    return 8*(x+2) + (y-1);
            }
            if(x + 1 <= 7 && y + 2 <= 7)
            {
                if(board[x + 1][y + 2] == 'n')
                    return 8*(x+1) + (y+2);
            }
            if(x + 1 <= 7 && y - 2 >= 0)
            {
                if(board[x + 1][y - 2] == 'n')
                    return 8*(x+1) + (y-2);
            }
            //up left
            for(int i = 1; i <= 7; i++)
            {
                if(x - i < 0 || y - i < 0)
                {
                    break;
                }
                if(board[x - i][y - i] != ' ')
                {
                    if(board[x - i][y - i] == 'b')
                    {
                        return 8*(x-i) + (y-i);
                    }
                    else if(board[x - i][y - i] == 'q')
                    {
                        attackers_index[0] = 8*(x-i) + (y-i);
                    }
                    else if(i == 1 && board[x - i][y - i] == 'k')
                    {
                        attackers_index[1] = 8*(x-i) + (y-i);
                    }
                    break;
                }
            }
            //up right
            for(int i = 1; i <= 7; i++)
            {
                if(x - i < 0 || y + i > 7)
                {
                    break;
                }
                if(board[x - i][y + i] != ' ')
                {
                    if(board[x - i][y + i] == 'b')
                    {
                        return 8*(x-i) + (y+i);
                    }
                    else if(board[x - i][y + i] == 'q')
                    {
                        attackers_index[0] = 8*(x-i) + (y+i);
                    }
                    else if(i == 1 && board[x - i][y + i] == 'k')
                    {
                        attackers_index[1] = 8*(x-i) + (y+i);
                    }
                    break;
                }
            }
            //down left
            for(int i = 1; i <= 7; i++)
            {
                if(x + i > 7 || y - i < 0)
                {
                    break;
                }
                if(board[x + i][y - i] != ' ')
                {
                    if(board[x + i][y - i] == 'b')
                    {
                        return 8*(x+i) + (y-i);
                    }
                    else if(board[x + i][y - i] == 'q')
                    {
                        attackers_index[0] = 8*(x+i) + (y-i);
                    }
                    else if(i == 1 && board[x + i][y - i] == 'k')
                    {
                        attackers_index[1] = 8*(x+i) + (y-i);
                    }
                    break;
                }
            }
            //down right
            for(int i = 1; i <= 7; i++)
            {
                if(x + i > 7 || y + i > 7)
                {
                    break;
                }
                if(board[x + i][y + i] != ' ')
                {
                    if(board[x + i][y + i] == 'b')
                    {
                        return 8*(x+i) + (y+i);
                    }
                    else if(board[x + i][y + i] == 'q')
                    {
                        attackers_index[0] = 8*(x+i) + (y+i);
                    }
                    else if(i == 1 && board[x + i][y + i] == 'k')
                    {
                        attackers_index[1] = 8*(x+i) + (y+i);
                    }
                    break;
                }
            }
            //up
            for(int i = x-1; i >= 0; i--)
            {
                if(board[i][y] != ' ')
                {
                    if(board[i][y] == 'r')
                    {
                        return 8*i + y;
                    }
                    else if(board[i][y] == 'q')
                    {
                        attackers_index[0] = 8*i + y;
                    }
                    else if(i == x-1 && board[i][y] == 'k')
                    {
                        attackers_index[1] = 8*i + y;
                    }
                    break;
                }
            }
            //left
            for(int i = y-1; i >= 0; i--)
            {
                if(board[x][i] != ' ')
                {
                    if(board[x][i] == 'r')
                    {
                        return 8*x + i;
                    }
                    else if(board[x][i] == 'q')
                    {
                        attackers_index[0] = 8*x + i;
                    }
                    else if(i == y-1 && board[x][i] == 'k')
                    {
                        attackers_index[1] = 8*x + i;
                    }

                    break;
                }
            }
            //right
            for(int i = y+1; i <= 7; i++)
            {
                if(board[x][i] != ' ')
                {
                    if(board[x][i] == 'r')
                    {
                        return 8*x + i;
                    }
                    else if(board[x][i] == 'q')
                    {
                        attackers_index[0] = 8*x + i;
                    }
                    else if(i == y+1 && board[x][i] == 'k')
                    {
                        attackers_index[1] = 8*x + i;
                    }
                    break;
                }
            }
            //down
            for(int i = x+1; i <= 7; i++)
            {
                if(board[i][y] != ' ')
                {
                    if(board[i][y] == 'r')
                    {
                        return 8*i + y;
                    }
                    else if(board[i][y] == 'q')
                    {
                        attackers_index[0] = 8*i + y;
                    }
                    else if(i == x+1 && board[i][y] == 'k')
                    {
                        attackers_index[1] = 8*i + y;
                    }
                    break;
                }
            }

            break;
        }
    }
    
    if(attackers_index[0] != -2)
    {
        return attackers_index[0];
    }
    else if(attackers_index[1] != -2)
    {
        return attackers_index[1];
    }

    return -1;
}


