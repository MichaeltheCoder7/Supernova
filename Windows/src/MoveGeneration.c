#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>
#include "MoveGeneration.h"
#include "Attack.h"
#include "CheckMove.h"
#include "Board.h"
#include "Search.h"


char positions[8][8][3] = {

                            {"a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"},
                            {"a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7"},
                            {"a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6"},
                            {"a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5"},
                            {"a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4"},
                            {"a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3"},
                            {"a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2"},
                            {"a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1"},

                            };


int knight_moves_x[8] = { -2, -2, -1, -1,  1,  1,  2,  2 };
int knight_moves_y[8] = { -1,  1, -2,  2, -2,  2, -1,  1 };

int king_moves_x[8] = { -1, -1, -1,  0,  0,  1,  1,  1 };
int king_moves_y[8] = { -1,  0,  1, -1,  1, -1,  0,  1 };

//save one side's all positions in an array of string
//need caller to declare: char opponent[16][3] array first and pass it in
//the array has a fixed size
//color = -1: white pieces
//color = 1: black pieces
static int all_positions(char board[8][8], char opponent[16][3], int color)
{
    int temp = 0;
    //get all white pieces locations
    switch(color)
    {
        case -1:
        {
            for(int x = 0; x < 8; x++)
            {
                for (int y = 0; y < 8; y++)
                {
                    if(isupper(board[x][y]))
                    {
                        strncpy(opponent[temp], positions[x][y], 3);
                        temp++;
                        
                    }
                }
            }
            break;
        }
        //get all black pieces locations
        case 1:
        {
            for(int x = 0; x < 8; x++)
            {
                for (int y = 0; y < 8; y++)
                {
                    if(islower(board[x][y]))
                    {
                        strncpy(opponent[temp], positions[x][y], 3);
                        temp++;
                        
                    }

                }
            }
            break;
        }
    }
    return temp;
}

//generate all pseudo-legal moves
int moveGen(char board[8][8], char all_moves[256][6], char op_cp[3], char op_np[3], int ksw, int qsw, int ksb, int qsb, int color)
{
    char piece_positions[16][3];
    int index = 0; 
    int length;
    char piece;
    int index_x;
    int index_y;
    char string[3];
    int x, y;

    switch(color)
    {
        case 1:
        {
            length = all_positions(board, piece_positions, 1);
            for(int i = 0; i < length; i++)
            {
                index_x = position_to_x(piece_positions[i]);
                index_y = position_to_y(piece_positions[i]);
                piece = board[index_x][index_y];
                switch(piece)
                {
                    case 'p':
                    {
                        for(y = index_y - 1; y < index_y + 2; y++)
                        {
                            if(y & 8) //skip when out of board
                            {
                                continue;
                            }
                            if(CheckMove_bpawn(board, index_y, index_x + 1, y, op_cp, op_np))  
                            { 
                                //promotions
                                if((index_x + 1) == 7)
                                {
                                    strncpy(string, piece_positions[i], 3);
                                    strncat(string, positions[index_x + 1][y], 3);
                                    strncpy(all_moves[index], string, 6);
                                    all_moves[index][4] = 'q';
                                    all_moves[index][5] = '\0';
                                    index++;
                                    strncpy(all_moves[index], string, 6);
                                    all_moves[index][4] = 'r';
                                    all_moves[index][5] = '\0';
                                    index++;
                                    strncpy(all_moves[index], string, 6);
                                    all_moves[index][4] = 'b';
                                    all_moves[index][5] = '\0';
                                    index++;
                                    strncpy(all_moves[index], string, 6);
                                    all_moves[index][4] = 'n';
                                    all_moves[index][5] = '\0';
                                    index++;
                                }
                                else
                                {
                                    strncpy(string, piece_positions[i], 3);
                                    strncat(string, positions[index_x + 1][y], 3);
                                    strncpy(all_moves[index], string, 6);
                                    index++;
                                }
                            }
                        }
                        //2 steps at starting position
                        if(index_x == 1)
                        {
                            if(board[2][index_y] == ' ' && board[3][index_y] == ' ')  
                            { 
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[3][index_y], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                        }
                        break;
                    }
                    case 'n':
                    {
                        for(int j = 0; j < 8; j++)
                        {
                            x = index_x + knight_moves_x[j];
                            y = index_y + knight_moves_y[j];
                            if(x & 8 || y & 8) //skips when out of board
                            {
                                continue;
                            }
                            if(board[x][y] == ' ' || board[x][y] == 'P' || board[x][y] == 'R' || board[x][y] == 'N' || board[x][y] == 'B' || board[x][y] == 'Q')  
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[x][y], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                        }
                        break;
                    }
                    case 'k':
                    {
                        //castling
                        if(index_x == 0 && index_y == 4)
                        {
                            if(CheckMove_bkingside(board, ksb))  
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[0][6], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(CheckMove_bqueenside(board, qsb))  
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[0][2], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                        }
                        for(int j = 0; j < 8; j++)
                        {
                            x = index_x + king_moves_x[j];
                            y = index_y + king_moves_y[j];
                            if(x & 8 || y & 8) //skip when out of board
                            {
                                continue;
                            }
                            if(board[x][y] == ' ' || board[x][y] == 'P' || board[x][y] == 'R' || board[x][y] == 'N' || board[x][y] == 'B' || board[x][y] == 'Q')  
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[x][y], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            
                        }
                        break;
                    }
                    case 'r':
                    {
                        //up
                        for(int j = index_x-1; j >= 0; j--)
                        {  
                            if(board[j][index_y] == ' ' || board[j][index_y] == 'P' || board[j][index_y] == 'R' || board[j][index_y] == 'N' || board[j][index_y] == 'B' || board[j][index_y] == 'Q')
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[j][index_y], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[j][index_y] != ' ')
                            {
                                break;
                            }
                        }
                        //left
                        for(int j = index_y-1; j >= 0; j--)
                        {
                            if(board[index_x][j] == ' ' || board[index_x][j] == 'P' || board[index_x][j] == 'R' || board[index_x][j] == 'N' || board[index_x][j] == 'B' || board[index_x][j] == 'Q')
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x][j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x][j] != ' ')
                            {
                                break;
                            }
                        }
                        //right
                        for(int j = index_y+1; j <= 7; j++)
                        {
                            if(board[index_x][j] == ' ' || board[index_x][j] == 'P' || board[index_x][j] == 'R' || board[index_x][j] == 'N' || board[index_x][j] == 'B' || board[index_x][j] == 'Q')  
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x][j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x][j] != ' ')
                            {
                                break;
                            }
                        }
                        //down
                        for(int j = index_x+1; j <= 7; j++)
                        {
                            if(board[j][index_y] == ' ' || board[j][index_y] == 'P' || board[j][index_y] == 'R' || board[j][index_y] == 'N' || board[j][index_y] == 'B' || board[j][index_y] == 'Q') 
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[j][index_y], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[j][index_y] != ' ')
                            {
                                break;
                            }
                        }
                        break;
                    }
                    case 'b':
                    {
                        //up left
                        for(int j = 1; j <= 7; j++)
                        {
                            if(index_x - j < 0 || index_y - j < 0)
                            {
                                break;
                            }
                            if(board[index_x - j][index_y - j] == ' ' || board[index_x - j][index_y - j] == 'P' || board[index_x - j][index_y - j] == 'R' || board[index_x - j][index_y - j] == 'N' || board[index_x - j][index_y - j] == 'B' || board[index_x - j][index_y - j] == 'Q')  
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x - j][index_y - j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
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
                            if(board[index_x - j][index_y + j] == ' ' || board[index_x - j][index_y + j] == 'P' || board[index_x - j][index_y + j] == 'R' || board[index_x - j][index_y + j] == 'N' || board[index_x - j][index_y + j] == 'B' || board[index_x - j][index_y + j] == 'Q') 
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x - j][index_y + j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
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
                            if(board[index_x + j][index_y - j] == ' ' || board[index_x + j][index_y - j] == 'P' || board[index_x + j][index_y - j] == 'R' || board[index_x + j][index_y - j] == 'N' || board[index_x + j][index_y - j] == 'B' || board[index_x + j][index_y - j] == 'Q')    
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x + j][index_y - j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
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
                            if(board[index_x + j][index_y + j] == ' ' || board[index_x + j][index_y + j] == 'P' || board[index_x + j][index_y + j] == 'R' || board[index_x + j][index_y + j] == 'N' || board[index_x + j][index_y + j] == 'B' || board[index_x + j][index_y + j] == 'Q')   
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x + j][index_y + j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x + j][index_y + j] != ' ')
                            {
                                break;
                            }
                        }
                        break;
                    }
                    case 'q':
                    {
                        //up left
                        for(int j = 1; j <= 7; j++)
                        {
                            if(index_x - j < 0 || index_y - j < 0)
                            {
                                break;
                            }
                            if(board[index_x - j][index_y - j] == ' ' || board[index_x - j][index_y - j] == 'P' || board[index_x - j][index_y - j] == 'R' || board[index_x - j][index_y - j] == 'N' || board[index_x - j][index_y - j] == 'B' || board[index_x - j][index_y - j] == 'Q')  
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x - j][index_y - j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x - j][index_y - j] != ' ')
                            {
                                break;
                            }
                        }
                        //up
                        for(int j = index_x-1; j >= 0; j--)
                        {
                            if(board[j][index_y] == ' ' || board[j][index_y] == 'P' || board[j][index_y] == 'R' || board[j][index_y] == 'N' || board[j][index_y] == 'B' || board[j][index_y] == 'Q') 
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[j][index_y], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
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
                            if(board[index_x - j][index_y + j] == ' ' || board[index_x - j][index_y + j] == 'P' || board[index_x - j][index_y + j] == 'R' || board[index_x - j][index_y + j] == 'N' || board[index_x - j][index_y + j] == 'B' || board[index_x - j][index_y + j] == 'Q') 
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x - j][index_y + j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x - j][index_y + j] != ' ')
                            {
                                break;
                            }
                        }
                        //left
                        for(int j = index_y-1; j >= 0; j--)
                        {
                            if(board[index_x][j] == ' ' || board[index_x][j] == 'P' || board[index_x][j] == 'R' || board[index_x][j] == 'N' || board[index_x][j] == 'B' || board[index_x][j] == 'Q')
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x][j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x][j] != ' ')
                            {
                                break;
                            }
                        }
                        //right
                        for(int j = index_y+1; j <= 7; j++)
                        {
                            if(board[index_x][j] == ' ' || board[index_x][j] == 'P' || board[index_x][j] == 'R' || board[index_x][j] == 'N' || board[index_x][j] == 'B' || board[index_x][j] == 'Q') 
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x][j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
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
                            if(board[index_x + j][index_y - j] == ' ' || board[index_x + j][index_y - j] == 'P' || board[index_x + j][index_y - j] == 'R' || board[index_x + j][index_y - j] == 'N' || board[index_x + j][index_y - j] == 'B' || board[index_x + j][index_y - j] == 'Q')  
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x + j][index_y - j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x + j][index_y - j] != ' ')
                            {
                                break;
                            }
                        }
                        //down
                        for(int j = index_x+1; j <= 7; j++)
                        {
                            if(board[j][index_y] == ' ' || board[j][index_y] == 'P' || board[j][index_y] == 'R' || board[j][index_y] == 'N' || board[j][index_y] == 'B' || board[j][index_y] == 'Q') 
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[j][index_y], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
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
                            if(board[index_x + j][index_y + j] == ' ' || board[index_x + j][index_y + j] == 'P' || board[index_x + j][index_y + j] == 'R' || board[index_x + j][index_y + j] == 'N' || board[index_x + j][index_y + j] == 'B' || board[index_x + j][index_y + j] == 'Q')   
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x + j][index_y + j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x + j][index_y + j] != ' ')
                            {
                                break;
                            }
                        }
                        break;
                    }
                }
            }
            break;
        }
        case -1:
        {
            length = all_positions(board, piece_positions, -1);
            for(int i = 0; i < length; i++)
            {
                index_x = position_to_x(piece_positions[i]);
                index_y = position_to_y(piece_positions[i]);
                piece = board[index_x][index_y];
                switch(piece)
                {
                    case 'P':
                    {
                        //2 steps at starting position
                        if(index_x == 6)
                        {
                            if(board[5][index_y] == ' ' && board[4][index_y] == ' ')  
                            { 
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[4][index_y], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                        }
                        for(y = index_y - 1; y < index_y + 2; y++)
                        {
                            if(y & 8) //skip when out of board
                            {
                                continue;
                            }
                            if(CheckMove_wpawn(board, index_y, index_x - 1, y, op_cp, op_np)) 
                            { 
                                //promotions
                                if((index_x - 1) == 0)
                                {
                                    strncpy(string, piece_positions[i], 3);
                                    strncat(string, positions[index_x - 1][y], 3);
                                    strncpy(all_moves[index], string, 6);
                                    all_moves[index][4] = 'q';
                                    all_moves[index][5] = '\0';
                                    index++;
                                    strncpy(all_moves[index], string, 6);
                                    all_moves[index][4] = 'r';
                                    all_moves[index][5] = '\0';
                                    index++;
                                    strncpy(all_moves[index], string, 6);
                                    all_moves[index][4] = 'b';
                                    all_moves[index][5] = '\0';
                                    index++;
                                    strncpy(all_moves[index], string, 6);
                                    all_moves[index][4] = 'n';
                                    all_moves[index][5] = '\0';
                                    index++;
                                }
                                else
                                {
                                    strncpy(string, piece_positions[i], 3);
                                    strncat(string, positions[index_x - 1][y], 3);
                                    strncpy(all_moves[index], string, 6);
                                    index++;
                                }
                            }
                        }
                        break;
                    }
                    case 'N':
                    {
                        for(int j = 0; j < 8; j++)
                        {
                            x = index_x + knight_moves_x[j];
                            y = index_y + knight_moves_y[j];
                            if(x & 8 || y & 8) //skip when out of board
                            {
                                continue;
                            }
                            if(board[x][y] == ' ' || board[x][y] == 'p' || board[x][y] == 'r' || board[x][y] == 'n' || board[x][y] == 'b' || board[x][y] == 'q')   
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[x][y], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                        }
                        break;
                    }
                    case 'K':
                    {
                        //castling
                        if(index_x == 7 && index_y == 4)
                        {
                            if(CheckMove_wkingside(board, ksw))  
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[7][6], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(CheckMove_wqueenside(board, qsw))  
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[7][2], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                        }
                        for(int j = 0; j < 8; j++)
                        {
                            x = index_x + king_moves_x[j];
                            y = index_y + king_moves_y[j];
                            if(x & 8 || y & 8) //skip when out of board
                            {
                                continue;
                            }
                            if(board[x][y] == ' ' || board[x][y] == 'p' || board[x][y] == 'r' || board[x][y] == 'n' || board[x][y] == 'b' || board[x][y] == 'q')   
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[x][y], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                        }
                        break;
                    }
                    case 'R':
                    {
                        //up
                        for(int j = index_x-1; j >= 0; j--)
                        {
                            if(board[j][index_y] == ' ' || board[j][index_y] == 'p' || board[j][index_y] == 'r' || board[j][index_y] == 'n' || board[j][index_y] == 'b' || board[j][index_y] == 'q') 
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[j][index_y], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[j][index_y] != ' ')
                            {
                                break;
                            }
                        }
                        //left
                        for(int j = index_y-1; j >= 0; j--)
                        {
                            if(board[index_x][j] == ' ' || board[index_x][j] == 'p' || board[index_x][j] == 'r' || board[index_x][j] == 'n' || board[index_x][j] == 'b' || board[index_x][j] == 'q') 
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x][j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x][j] != ' ')
                            {
                                break;
                            }
                        }
                        //right
                        for(int j = index_y+1; j <= 7; j++)
                        {
                            if(board[index_x][j] == ' ' || board[index_x][j] == 'p' || board[index_x][j] == 'r' || board[index_x][j] == 'n' || board[index_x][j] == 'b' || board[index_x][j] == 'q')  
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x][j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x][j] != ' ')
                            {
                                break;
                            }
                        }
                        //down
                        for(int j = index_x+1; j <= 7; j++)
                        {
                            if(board[j][index_y] == ' ' || board[j][index_y] == 'p' || board[j][index_y] == 'r' || board[j][index_y] == 'n' || board[j][index_y] == 'b' || board[j][index_y] == 'q') 
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[j][index_y], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[j][index_y] != ' ')
                            {
                                break;
                            }
                        }
                        break;
                    }
                    case 'B':
                    {
                        //up left
                        for(int j = 1; j <= 7; j++)
                        {
                            if(index_x - j < 0 || index_y - j < 0)
                            {
                                break;
                            }
                            if(board[index_x - j][index_y - j] == ' ' || board[index_x - j][index_y - j] == 'p' || board[index_x - j][index_y - j] == 'r' || board[index_x - j][index_y - j] == 'n' || board[index_x - j][index_y - j] == 'b' || board[index_x - j][index_y - j] == 'q')  
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x - j][index_y - j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
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
                            if(board[index_x - j][index_y + j] == ' ' || board[index_x - j][index_y + j] == 'p' || board[index_x - j][index_y + j] == 'r' || board[index_x - j][index_y + j] == 'n' || board[index_x - j][index_y + j] == 'b' || board[index_x - j][index_y + j] == 'q')   
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x - j][index_y + j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
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
                            if(board[index_x + j][index_y - j] == ' ' || board[index_x + j][index_y - j] == 'p' || board[index_x + j][index_y - j] == 'r' || board[index_x + j][index_y - j] == 'n' || board[index_x + j][index_y - j] == 'b' || board[index_x + j][index_y - j] == 'q')   
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x + j][index_y - j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
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
                            if(board[index_x + j][index_y + j] == ' ' || board[index_x + j][index_y + j] == 'p' || board[index_x + j][index_y + j] == 'r' || board[index_x + j][index_y + j] == 'n' || board[index_x + j][index_y + j] == 'b' || board[index_x + j][index_y + j] == 'q')   
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x + j][index_y + j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x + j][index_y + j] != ' ')
                            {
                                break;
                            }
                        }
                        break;
                    }
                    case 'Q':
                    {
                        //up left
                        for(int j = 1; j <= 7; j++)
                        {
                            if(index_x - j < 0 || index_y - j < 0)
                            {
                                break;
                            }
                            if(board[index_x - j][index_y - j] == ' ' || board[index_x - j][index_y - j] == 'p' || board[index_x - j][index_y - j] == 'r' || board[index_x - j][index_y - j] == 'n' || board[index_x - j][index_y - j] == 'b' || board[index_x - j][index_y - j] == 'q')  
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x - j][index_y - j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x - j][index_y - j] != ' ')
                            {
                                break;
                            }
                        }
                        //up
                        for(int j = index_x-1; j >= 0; j--)
                        {
                            if(board[j][index_y] == ' ' || board[j][index_y] == 'p' || board[j][index_y] == 'r' || board[j][index_y] == 'n' || board[j][index_y] == 'b' || board[j][index_y] == 'q')  
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[j][index_y], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
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
                            if(board[index_x - j][index_y + j] == ' ' || board[index_x - j][index_y + j] == 'p' || board[index_x - j][index_y + j] == 'r' || board[index_x - j][index_y + j] == 'n' || board[index_x - j][index_y + j] == 'b' || board[index_x - j][index_y + j] == 'q')   
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x - j][index_y + j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x - j][index_y + j] != ' ')
                            {
                                break;
                            }
                        }
                        //left
                        for(int j = index_y-1; j >= 0; j--)
                        {
                            if(board[index_x][j] == ' ' || board[index_x][j] == 'p' || board[index_x][j] == 'r' || board[index_x][j] == 'n' || board[index_x][j] == 'b' || board[index_x][j] == 'q')   
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x][j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x][j] != ' ')
                            {
                                break;
                            }
                        }
                        //right
                        for(int j = index_y+1; j <= 7; j++)
                        {
                            if(board[index_x][j] == ' ' || board[index_x][j] == 'p' || board[index_x][j] == 'r' || board[index_x][j] == 'n' || board[index_x][j] == 'b' || board[index_x][j] == 'q')   
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x][j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
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
                            if(board[index_x + j][index_y - j] == ' ' || board[index_x + j][index_y - j] == 'p' || board[index_x + j][index_y - j] == 'r' || board[index_x + j][index_y - j] == 'n' || board[index_x + j][index_y - j] == 'b' || board[index_x + j][index_y - j] == 'q')   
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x + j][index_y - j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x + j][index_y - j] != ' ')
                            {
                                break;
                            }
                        }
                        //down
                        for(int j = index_x+1; j <= 7; j++)
                        {
                            if(board[j][index_y] == ' ' || board[j][index_y] == 'p' || board[j][index_y] == 'r' || board[j][index_y] == 'n' || board[j][index_y] == 'b' || board[j][index_y] == 'q') 
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[j][index_y], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
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
                            if(board[index_x + j][index_y + j] == ' ' || board[index_x + j][index_y + j] == 'p' || board[index_x + j][index_y + j] == 'r' || board[index_x + j][index_y + j] == 'n' || board[index_x + j][index_y + j] == 'b' || board[index_x + j][index_y + j] == 'q')   
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x + j][index_y + j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x + j][index_y + j] != ' ')
                            {
                                break;
                            }
                        }
                        break;
                    }
                }
            }
            break;
        }
        
    }
    
    return index;
}

//generate caputures and queen promotion
int captureGen(char board[8][8], char all_moves[256][6], char op_cp[3], char op_np[3], int color)
{
    char piece_positions[16][3];
    int index = 0; 
    int length;
    char piece;
    int index_x;
    int index_y;
    char string[3];
    int x, y;

    switch(color)
    {
        case 1:
        {
            length = all_positions(board, piece_positions, 1);
            for(int i = 0; i < length; i++)
            {
                index_x = position_to_x(piece_positions[i]);
                index_y = position_to_y(piece_positions[i]);
                piece = board[index_x][index_y];
                switch(piece)
                {
                    case 'p':
                    {
                        if((index_y - 1) >= 0 && CheckCapture_bpawn(board, index_x + 1, index_y - 1, op_cp, op_np)) 
                        { 
                            strncpy(string, piece_positions[i], 3);
                            strncat(string, positions[index_x + 1][index_y - 1], 3);
                            strncpy(all_moves[index], string, 6);
                            index++;
                        }
                        //promotion
                        if(index_x == 6 && board[7][index_y] == ' ')
                        {
                            strncpy(string, piece_positions[i], 3);
                            strncat(string, positions[7][index_y], 3);
                            strncpy(all_moves[index], string, 6);
                            index++;
                        } 
                        if((index_y + 1) <= 7 && CheckCapture_bpawn(board, index_x + 1, index_y + 1, op_cp, op_np)) 
                        { 
                            strncpy(string, piece_positions[i], 3);
                            strncat(string, positions[index_x + 1][index_y + 1], 3);
                            strncpy(all_moves[index], string, 6);
                            index++;
                        }

                        break;
                    }
                    case 'n':
                    {
                        for(int j = 0; j < 8; j++)
                        {
                            x = index_x + knight_moves_x[j];
                            y = index_y + knight_moves_y[j];
                            if(x & 8 || y & 8) //skip when out of board
                            {
                                continue;
                            }
                            if(board[x][y] == 'P' || board[x][y] == 'R' || board[x][y] == 'N' || board[x][y] == 'B' || board[x][y] == 'Q')
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[x][y], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                        }
                        break;
                    }
                    case 'k':
                    {
                        for(int j = 0; j < 8; j++)
                        {
                            x = index_x + king_moves_x[j];
                            y = index_y + king_moves_y[j];
                            if(x & 8 || y & 8) //skip when out of board
                            {
                                continue;
                            }
                            if(board[x][y] == 'P' || board[x][y] == 'R' || board[x][y] == 'N' || board[x][y] == 'B' || board[x][y] == 'Q')  
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[x][y], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                        }
                        
                        break;
                    }
                    case 'r':
                    {
                        //up
                        for(int j = index_x-1; j >= 0; j--)
                        {  
                            if(board[j][index_y] == 'P' || board[j][index_y] == 'R' || board[j][index_y] == 'N' || board[j][index_y] == 'B' || board[j][index_y] == 'Q')
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[j][index_y], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[j][index_y] != ' ')
                            {
                                break;
                            }
                        }
                        //left
                        for(int j = index_y-1; j >= 0; j--)
                        {
                            if(board[index_x][j] == 'P' || board[index_x][j] == 'R' || board[index_x][j] == 'N' || board[index_x][j] == 'B' || board[index_x][j] == 'Q')
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x][j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x][j] != ' ')
                            {
                                break;
                            }
                        }
                        //right
                        for(int j = index_y+1; j <= 7; j++)
                        {
                            if(board[index_x][j] == 'P' || board[index_x][j] == 'R' || board[index_x][j] == 'N' || board[index_x][j] == 'B' || board[index_x][j] == 'Q')  
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x][j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x][j] != ' ')
                            {
                                break;
                            }
                        }
                        //down
                        for(int j = index_x+1; j <= 7; j++)
                        {
                            if(board[j][index_y] == 'P' || board[j][index_y] == 'R' || board[j][index_y] == 'N' || board[j][index_y] == 'B' || board[j][index_y] == 'Q') 
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[j][index_y], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[j][index_y] != ' ')
                            {
                                break;
                            }
                        }
                        break;
                    }
                    case 'b':
                    {
                        //up left
                        for(int j = 1; j <= 7; j++)
                        {
                            if(index_x - j < 0 || index_y - j < 0)
                            {
                                break;
                            }
                            if(board[index_x - j][index_y - j] == 'P' || board[index_x - j][index_y - j] == 'R' || board[index_x - j][index_y - j] == 'N' || board[index_x - j][index_y - j] == 'B' || board[index_x - j][index_y - j] == 'Q')  
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x - j][index_y - j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
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
                            if(board[index_x - j][index_y + j] == 'P' || board[index_x - j][index_y + j] == 'R' || board[index_x - j][index_y + j] == 'N' || board[index_x - j][index_y + j] == 'B' || board[index_x - j][index_y + j] == 'Q') 
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x - j][index_y + j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
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
                            if(board[index_x + j][index_y - j] == 'P' || board[index_x + j][index_y - j] == 'R' || board[index_x + j][index_y - j] == 'N' || board[index_x + j][index_y - j] == 'B' || board[index_x + j][index_y - j] == 'Q')    
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x + j][index_y - j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
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
                            if(board[index_x + j][index_y + j] == 'P' || board[index_x + j][index_y + j] == 'R' || board[index_x + j][index_y + j] == 'N' || board[index_x + j][index_y + j] == 'B' || board[index_x + j][index_y + j] == 'Q')   
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x + j][index_y + j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x + j][index_y + j] != ' ')
                            {
                                break;
                            }
                        }
                        break;
                    }
                    case 'q':
                    {
                        //up left
                        for(int j = 1; j <= 7; j++)
                        {
                            if(index_x - j < 0 || index_y - j < 0)
                            {
                                break;
                            }
                            if(board[index_x - j][index_y - j] == 'P' || board[index_x - j][index_y - j] == 'R' || board[index_x - j][index_y - j] == 'N' || board[index_x - j][index_y - j] == 'B' || board[index_x - j][index_y - j] == 'Q')  
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x - j][index_y - j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x - j][index_y - j] != ' ')
                            {
                                break;
                            }
                        }
                        //up
                        for(int j = index_x-1; j >= 0; j--)
                        {
                            if(board[j][index_y] == 'P' || board[j][index_y] == 'R' || board[j][index_y] == 'N' || board[j][index_y] == 'B' || board[j][index_y] == 'Q') 
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[j][index_y], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
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
                            if(board[index_x - j][index_y + j] == 'P' || board[index_x - j][index_y + j] == 'R' || board[index_x - j][index_y + j] == 'N' || board[index_x - j][index_y + j] == 'B' || board[index_x - j][index_y + j] == 'Q') 
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x - j][index_y + j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x - j][index_y + j] != ' ')
                            {
                                break;
                            }
                        }
                        //left
                        for(int j = index_y-1; j >= 0; j--)
                        {
                            if(board[index_x][j] == 'P' || board[index_x][j] == 'R' || board[index_x][j] == 'N' || board[index_x][j] == 'B' || board[index_x][j] == 'Q')
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x][j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x][j] != ' ')
                            {
                                break;
                            }
                        }
                        //right
                        for(int j = index_y+1; j <= 7; j++)
                        {
                            if(board[index_x][j] == 'P' || board[index_x][j] == 'R' || board[index_x][j] == 'N' || board[index_x][j] == 'B' || board[index_x][j] == 'Q') 
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x][j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
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
                            if(board[index_x + j][index_y - j] == 'P' || board[index_x + j][index_y - j] == 'R' || board[index_x + j][index_y - j] == 'N' || board[index_x + j][index_y - j] == 'B' || board[index_x + j][index_y - j] == 'Q')  
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x + j][index_y - j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x + j][index_y - j] != ' ')
                            {
                                break;
                            }
                        }
                        //down
                        for(int j = index_x+1; j <= 7; j++)
                        {
                            if(board[j][index_y] == 'P' || board[j][index_y] == 'R' || board[j][index_y] == 'N' || board[j][index_y] == 'B' || board[j][index_y] == 'Q') 
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[j][index_y], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
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
                            if(board[index_x + j][index_y + j] == 'P' || board[index_x + j][index_y + j] == 'R' || board[index_x + j][index_y + j] == 'N' || board[index_x + j][index_y + j] == 'B' || board[index_x + j][index_y + j] == 'Q')   
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x + j][index_y + j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x + j][index_y + j] != ' ')
                            {
                                break;
                            }
                        }
                        break;
                    }
                }
            }
            break;
        }
        case -1:
        {
            length = all_positions(board, piece_positions, -1);
            for(int i = 0; i < length; i++)
            {
                index_x = position_to_x(piece_positions[i]);
                index_y = position_to_y(piece_positions[i]);
                piece = board[index_x][index_y];
                switch(piece)
                {
                    case 'P':
                    {
                        if((index_y - 1) >= 0 && CheckCapture_wpawn(board, index_x - 1, index_y - 1, op_cp, op_np)) 
                        { 
                            strncpy(string, piece_positions[i], 3);
                            strncat(string, positions[index_x - 1][index_y - 1], 3);
                            strncpy(all_moves[index], string, 6);
                            index++;
                        }
                        //promotion
                        if(index_x == 1 && board[0][index_y] == ' ')
                        {
                            strncpy(string, piece_positions[i], 3);
                            strncat(string, positions[0][index_y], 3);
                            strncpy(all_moves[index], string, 6);
                            index++;
                        } 
                        if((index_y + 1) <= 7 && CheckCapture_wpawn(board, index_x - 1, index_y + 1, op_cp, op_np)) 
                        { 
                            strncpy(string, piece_positions[i], 3);
                            strncat(string, positions[index_x - 1][index_y + 1], 3);
                            strncpy(all_moves[index], string, 6);
                            index++;
                        }
                        break;
                    }
                    case 'N':
                    {
                        for(int j = 0; j < 8; j++)
                        {
                            x = index_x + knight_moves_x[j];
                            y = index_y + knight_moves_y[j];
                            if(x & 8 || y & 8) //skip when out of board
                            {
                                continue;
                            }
                            if(board[x][y] == 'p' || board[x][y] == 'r' || board[x][y] == 'n' || board[x][y] == 'b' || board[x][y] == 'q')
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[x][y], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                        }
                        break;
                    }
                    case 'K':
                    {
                        for(int j = 0; j < 8; j++)
                        {
                            x = index_x + king_moves_x[j];
                            y = index_y + king_moves_y[j];
                            if(x & 8 || y & 8) //skip when out of board
                            {
                                continue;
                            }
                            if(board[x][y] == 'p' || board[x][y] == 'r' || board[x][y] == 'n' || board[x][y] == 'b' || board[x][y] == 'q')
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[x][y], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                        }
                        break;
                    }
                    case 'R':
                    {
                        //up
                        for(int j = index_x-1; j >= 0; j--)
                        {
                            if(board[j][index_y] == 'p' || board[j][index_y] == 'r' || board[j][index_y] == 'n' || board[j][index_y] == 'b' || board[j][index_y] == 'q') 
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[j][index_y], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[j][index_y] != ' ')
                            {
                                break;
                            }
                        }
                        //left
                        for(int j = index_y-1; j >= 0; j--)
                        {
                            if(board[index_x][j] == 'p' || board[index_x][j] == 'r' || board[index_x][j] == 'n' || board[index_x][j] == 'b' || board[index_x][j] == 'q') 
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x][j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x][j] != ' ')
                            {
                                break;
                            }
                        }
                        //right
                        for(int j = index_y+1; j <= 7; j++)
                        {
                            if(board[index_x][j] == 'p' || board[index_x][j] == 'r' || board[index_x][j] == 'n' || board[index_x][j] == 'b' || board[index_x][j] == 'q')  
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x][j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x][j] != ' ')
                            {
                                break;
                            }
                        }
                        //down
                        for(int j = index_x+1; j <= 7; j++)
                        {
                            if(board[j][index_y] == 'p' || board[j][index_y] == 'r' || board[j][index_y] == 'n' || board[j][index_y] == 'b' || board[j][index_y] == 'q') 
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[j][index_y], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[j][index_y] != ' ')
                            {
                                break;
                            }
                        }
                        break;
                    }
                    case 'B':
                    {
                        //up left
                        for(int j = 1; j <= 7; j++)
                        {
                            if(index_x - j < 0 || index_y - j < 0)
                            {
                                break;
                            }
                            if(board[index_x - j][index_y - j] == 'p' || board[index_x - j][index_y - j] == 'r' || board[index_x - j][index_y - j] == 'n' || board[index_x - j][index_y - j] == 'b' || board[index_x - j][index_y - j] == 'q')  
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x - j][index_y - j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
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
                            if(board[index_x - j][index_y + j] == 'p' || board[index_x - j][index_y + j] == 'r' || board[index_x - j][index_y + j] == 'n' || board[index_x - j][index_y + j] == 'b' || board[index_x - j][index_y + j] == 'q')   
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x - j][index_y + j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
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
                            if(board[index_x + j][index_y - j] == 'p' || board[index_x + j][index_y - j] == 'r' || board[index_x + j][index_y - j] == 'n' || board[index_x + j][index_y - j] == 'b' || board[index_x + j][index_y - j] == 'q')   
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x + j][index_y - j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
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
                            if(board[index_x + j][index_y + j] == 'p' || board[index_x + j][index_y + j] == 'r' || board[index_x + j][index_y + j] == 'n' || board[index_x + j][index_y + j] == 'b' || board[index_x + j][index_y + j] == 'q')   
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x + j][index_y + j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x + j][index_y + j] != ' ')
                            {
                                break;
                            }
                        }
                        break;
                    }
                    case 'Q':
                    {
                        //up left
                        for(int j = 1; j <= 7; j++)
                        {
                            if(index_x - j < 0 || index_y - j < 0)
                            {
                                break;
                            }
                            if(board[index_x - j][index_y - j] == 'p' || board[index_x - j][index_y - j] == 'r' || board[index_x - j][index_y - j] == 'n' || board[index_x - j][index_y - j] == 'b' || board[index_x - j][index_y - j] == 'q')  
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x - j][index_y - j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x - j][index_y - j] != ' ')
                            {
                                break;
                            }
                        }
                        //up
                        for(int j = index_x-1; j >= 0; j--)
                        {
                            if(board[j][index_y] == 'p' || board[j][index_y] == 'r' || board[j][index_y] == 'n' || board[j][index_y] == 'b' || board[j][index_y] == 'q')  
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[j][index_y], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
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
                            if(board[index_x - j][index_y + j] == 'p' || board[index_x - j][index_y + j] == 'r' || board[index_x - j][index_y + j] == 'n' || board[index_x - j][index_y + j] == 'b' || board[index_x - j][index_y + j] == 'q')   
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x - j][index_y + j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x - j][index_y + j] != ' ')
                            {
                                break;
                            }
                        }
                        //left
                        for(int j = index_y-1; j >= 0; j--)
                        {
                            if(board[index_x][j] == 'p' || board[index_x][j] == 'r' || board[index_x][j] == 'n' || board[index_x][j] == 'b' || board[index_x][j] == 'q')   
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x][j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x][j] != ' ')
                            {
                                break;
                            }
                        }
                        //right
                        for(int j = index_y+1; j <= 7; j++)
                        {
                            if(board[index_x][j] == 'p' || board[index_x][j] == 'r' || board[index_x][j] == 'n' || board[index_x][j] == 'b' || board[index_x][j] == 'q')   
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x][j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
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
                            if(board[index_x + j][index_y - j] == 'p' || board[index_x + j][index_y - j] == 'r' || board[index_x + j][index_y - j] == 'n' || board[index_x + j][index_y - j] == 'b' || board[index_x + j][index_y - j] == 'q')   
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x + j][index_y - j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x + j][index_y - j] != ' ')
                            {
                                break;
                            }
                        }
                        //down
                        for(int j = index_x+1; j <= 7; j++)
                        {
                            if(board[j][index_y] == 'p' || board[j][index_y] == 'r' || board[j][index_y] == 'n' || board[j][index_y] == 'b' || board[j][index_y] == 'q') 
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[j][index_y], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
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
                            if(board[index_x + j][index_y + j] == 'p' || board[index_x + j][index_y + j] == 'r' || board[index_x + j][index_y + j] == 'n' || board[index_x + j][index_y + j] == 'b' || board[index_x + j][index_y + j] == 'q')   
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x + j][index_y + j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x + j][index_y + j] != ' ')
                            {
                                break;
                            }
                        }
                        break;
                    }
                }
            }
            break;
        }
    }
    
    return index;
}

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

static inline int partition(int arr[], int low, int high, char moves[256][6]) 
{ 
    int pivot = arr[low];    // pivot 
    int i = low;  // Index of smaller element 
  
    for (int j = low + 1; j <= high; j++) 
    { 
        // If current element is smaller than the pivot 
        if (arr[j] > pivot) 
        { 
            i++;    // increment index of smaller element 
            swap(&arr[i], &arr[j]); 
            swapstr(moves[i], moves[j]);
        } 
    } 
    swap(&arr[i], &arr[low]);
    swapstr(moves[i], moves[low]); 
    return i; 
} 
  
//QuickSort in descending order
//arr[] --> Array to be sorted, 
//low  --> Starting index, 
//high  --> Ending index 
static inline void quickSort(int arr[], int low, int high, char moves[256][6]) 
{ 
    if (low < high) 
    { 
        // pi is partitioning index, arr[p] is now 
        // at right place 
        int pi = partition(arr, low, high, moves); 
  
        // Separately sort elements before 
        // partition and after partition 
        quickSort(arr, low, pi, moves); 
        quickSort(arr, pi + 1, high, moves); 
    } 
}

//move-ordering:
void move_ordering(char board[8][8], char moves[256][6], int length, char pv_move[6], int ply, int color)
{
    int sort[length];
    char cur_p[3];
    char new_p[3];
    char op_piece;
    char piece;
    int mvv;
    int lva;

    switch(color)
    {
        case 1:
        {
            for(int x = 0; x < length; x++)
            {
                mvv = lva = 0;
                sscanf(moves[x], "%2s%2s", cur_p, new_p);
                op_piece = position_to_piece(board, new_p);
                if(!strncmp(pv_move, moves[x], 5))
                {
                    sort[x] = 900000;
                }
                else if(op_piece != ' ')
                {
                    piece = position_to_piece(board, cur_p);
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
                    }
                    sort[x] = mvv + lva + 800000;

                }
                else if(!strncmp(killers[ply][0], moves[x], 5))
                {
                    sort[x] = 800150;
                }
                else if(!strncmp(killers[ply][1], moves[x], 5))
                {
                    sort[x] = 800140;
                }
                else
                {
                    sort[x] = history[1][8*position_to_x(cur_p) + position_to_y(cur_p)][8*position_to_x(new_p) + position_to_y(new_p)];
                }
            }
            break;
        }
        case -1:
        {
            for(int x = 0; x < length; x++)
            {
                mvv = lva = 0;
                sscanf(moves[x], "%2s%2s", cur_p, new_p);
                op_piece = position_to_piece(board, new_p);
                if(!strncmp(pv_move, moves[x], 5))
                {
                    sort[x] = 900000;
                }
                else if(op_piece != ' ')
                {
                    piece = position_to_piece(board, cur_p);
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
                    }
                    sort[x] = mvv + lva + 800000;
                }
                else if(!strncmp(killers[ply][0], moves[x], 5))
                {
                    sort[x] = 800150;
                }
                else if(!strncmp(killers[ply][1], moves[x], 5))
                {
                    sort[x] = 800140;
                }
                else
                {
                    sort[x] = history[0][8*position_to_x(cur_p) + position_to_y(cur_p)][8*position_to_x(new_p) + position_to_y(new_p)];
                }
            }
            break;
        }
    }

    quickSort(sort, 0, length - 1, moves);
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

static inline bool good_capture(char board[8][8], char own_piece, char cap_piece, char np[3], int color)
{
    //captures by pawn is good
    if(own_piece == 'P' || own_piece == 'p')
    {
        return true;
    }

    //check if lower takes higher or equal
    if(piece_value(cap_piece) >= piece_value(own_piece))
    {
        return true;
    }

    //check if piece captured is defended
    if(!isThreatened(board, np, color))
    {
        return true;
    }

    return false;
}

//move-ordering:
void cap_ordering(char board[8][8], char moves[100][6], int length, int color)
{
    int sort[length];
    char cur_p[3];
    char new_p[3];
    char op_piece;
    char piece;
    int mvv;
    int lva;

    switch(color)
    {
        case 1:
        {
            for(int x = 0; x < length; x++)
            {
                mvv = lva = 0;
                sscanf(moves[x], "%2s%2s", cur_p, new_p);
                op_piece = position_to_piece(board, new_p);
                piece = position_to_piece(board, cur_p);
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
                    default:
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
                    default:
                        mvv = 0;
                        break;
                }
                sort[x] = mvv + lva;
                
                if(good_capture(board, piece, op_piece, new_p, color))
                {
                    sort[x] += 600;
                }
            }
            break;
        }
        case -1:
        {
            for(int x = 0; x < length; x++)
            {
                mvv = lva = 0;
                sscanf(moves[x], "%2s%2s", cur_p, new_p);
                op_piece = position_to_piece(board, new_p);
                piece = position_to_piece(board, cur_p);
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
                    default:
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
                    default:
                        mvv = 0;
                        break;
                }
                sort[x] = mvv + lva;
                
                if(good_capture(board, piece, op_piece, new_p, color))
                {
                    sort[x] += 600;
                }
            }
            break;
        }
    }
    
    quickSort(sort, 0, length - 1, moves);
}

//static exchange evaluation for quiescence search
int SEE(char board[8][8], char location[3], int color)
{
    char board_copy[8][8];
    int value = 0;
    int x;
    int y;
    int attacker_index = get_smallest_attacker(board, location, color);
    char piece;
    int turn = 1;
    bool king_attack = false;
    memcpy(board_copy, board, sizeof(board_copy));

    while(attacker_index != -1)
    {
        x = attacker_index / 8;
        y = attacker_index % 8;
        
        if(board_copy[x][y] == 'K' || board_copy[x][y] == 'k')
            king_attack = true;            

        piece = position_to_piece(board_copy, location);
        make_move(positions[x][y], location, position_to_piece(board_copy, positions[x][y]), piece, board_copy);
        color = -color;
        attacker_index = get_smallest_attacker(board_copy, location, color);
        
        //stop when king is captured
        if(king_attack && attacker_index != -1)
            break;
        
        value -= turn * piece_value(piece);
        turn = -turn;
    }
    return value;
}