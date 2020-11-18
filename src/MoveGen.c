#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>
#include "MoveGen.h"
#include "CheckMove.h"
#include "OrderMove.h"
#include "Board.h"
#include "Move.h"

const int knight_moves_x[8] = { -2, -2, -1, -1,  1,  1,  2,  2 };
const int knight_moves_y[8] = { -1,  1, -2,  2, -2,  2, -1,  1 };
const int king_moves_x[8] = { -1, -1, -1,  0,  0,  1,  1,  1 };
const int king_moves_y[8] = { -1,  0,  1, -1,  1, -1,  0,  1 };

//generate all pseudo-legal moves
//generate scores for each move
int moveGen(BOARD *pos, MOVE all_moves[256], int sort[256], int ply, int color)
{
    int index = 0; 
    int index_x;
    int index_y;
    int x, y;
    int origin;

    switch(color)
    {
        //black pieces
        case 1:
        {
            //pawn
            for(int i = 0; i < pos->piece_count[bP]; i++)
            {
                origin = pos->piece_list[bP][i];
                index_x = origin / 8;
                index_y = origin % 8;
                //capture
                for(y = index_y - 1; y < index_y + 2; y += 2)
                {
                    if(y & 8) //skip when out of board
                    {
                        continue;
                    }
                    if(CheckCapture_bpawn(pos, index_x + 1, y))  
                    { 
                        //promotions
                        if(index_x == 6)
                        {
                            all_moves[index].from = origin;
                            all_moves[index].to = 8*(index_x+1)+y;
                            all_moves[index].promotion = 'q';
                            sort[index] = bCapMove_score('p', pos->board[index_x+1][y], pos->board, index_x, index_y, index_x+1, y) + 500;
                            index++;
                            all_moves[index].from = origin;
                            all_moves[index].to = 8*(index_x+1)+y;
                            all_moves[index].promotion = 'r';
                            sort[index] = UNDERPROM;
                            index++;
                            all_moves[index].from = origin;
                            all_moves[index].to = 8*(index_x+1)+y;
                            all_moves[index].promotion = 'b';
                            sort[index] = UNDERPROM;
                            index++;
                            all_moves[index].from = origin;
                            all_moves[index].to = 8*(index_x+1)+y;
                            all_moves[index].promotion = 'n';
                            sort[index] = UNDERPROM;
                            index++;
                        }
                        else
                        {
                            all_moves[index].from = origin;
                            all_moves[index].to = 8*(index_x+1)+y;
                            all_moves[index].promotion = ' ';
                            sort[index] = bCapMove_score('p', pos->board[index_x+1][y], pos->board, index_x, index_y, index_x+1, y);
                            index++;
                        }
                    }
                }
                //1 step
                if(pos->board[index_x + 1][index_y] == ' ')
                {
                    //promotions
                    if(index_x == 6)
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*(index_x+1)+index_y;
                        all_moves[index].promotion = 'q';
                        sort[index] = PROMOTION;
                        index++;
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*(index_x+1)+index_y;
                        all_moves[index].promotion = 'r';
                        sort[index] = UNDERPROM;
                        index++;
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*(index_x+1)+index_y;
                        all_moves[index].promotion = 'b';
                        sort[index] = UNDERPROM;
                        index++;
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*(index_x+1)+index_y;
                        all_moves[index].promotion = 'n';
                        sort[index] = UNDERPROM;
                        index++;
                    }
                    else
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*(index_x+1)+index_y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(&all_moves[index], origin, index_x+1, index_y, ply, color);
                        index++;
                    }
                }
                //2 steps at starting position
                if(index_x == 1)
                {
                    if(pos->board[2][index_y] == ' ' && pos->board[3][index_y] == ' ')  
                    { 
                        all_moves[index].from = origin;
                        all_moves[index].to = 24+index_y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(&all_moves[index], origin, 3, index_y, ply, color);
                        index++;
                    }
                }
            }
            //knight
            for(int i = 0; i < pos->piece_count[bN]; i++)
            {
                origin = pos->piece_list[bN][i];
                index_x = origin / 8;
                index_y = origin % 8;
                for(int j = 0; j < 8; j++)
                {
                    x = index_x + knight_moves_x[j];
                    y = index_y + knight_moves_y[j];
                    if(x & 8 || y & 8) //skip when out of board
                    {
                        continue;
                    }
                    if(pos->board[x][y] == ' ')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(&all_moves[index], origin, x, y, ply, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        all_moves[index].promotion = ' ';
                        sort[index] = bCapMove_score('n', pos->board[x][y], pos->board, index_x, index_y, x, y);
                        index++;
                    }
                }
            }
            //king
            origin = pos->piece_list[bK][0];
            index_x = origin / 8;
            index_y = origin % 8;
            //castling
            if(index_x == 0 && index_y == 4)
            {
                if(CheckMove_bkingside(pos))  
                {  
                    all_moves[index].from = origin;
                    all_moves[index].to = 6;
                    all_moves[index].promotion = ' ';
                    sort[index] = CASTLING;
                    index++;
                }
                if(CheckMove_bqueenside(pos))  
                {  
                    all_moves[index].from = origin;
                    all_moves[index].to = 2;
                    all_moves[index].promotion = ' ';
                    sort[index] = CASTLING;
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
                if(pos->board[x][y] == ' ') 
                {  
                    all_moves[index].from = origin;
                    all_moves[index].to = 8*x+y;
                    all_moves[index].promotion = ' ';
                    sort[index] = quietMove_score(&all_moves[index], origin, x, y, ply, color);
                    index++;
                }
                else if(pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q') 
                {  
                    all_moves[index].from = origin;
                    all_moves[index].to = 8*x+y;
                    all_moves[index].promotion = ' ';
                    sort[index] = bCapMove_score('k', pos->board[x][y], pos->board, index_x, index_y, x, y);
                    index++;
                }
            }
            //rook
            for(int i = 0; i < pos->piece_count[bR]; i++)
            {
                origin = pos->piece_list[bR][i];
                index_x = pos->piece_list[bR][i] / 8;
                index_y = pos->piece_list[bR][i] % 8;
                //up
                for(int j = index_x-1; j >= 0; j--)
                {  
                    if(pos->board[j][index_y] == ' ')
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*j+index_y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(&all_moves[index], origin, j, index_y, ply, color);
                        index++;
                    }
                    else if(pos->board[j][index_y] == 'P' || pos->board[j][index_y] == 'R' || pos->board[j][index_y] == 'N' || pos->board[j][index_y] == 'B' ||
                            pos->board[j][index_y] == 'Q')
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*j+index_y;
                        all_moves[index].promotion = ' ';
                        sort[index] = bCapMove_score('r', pos->board[j][index_y], pos->board, index_x, index_y, j, index_y);
                        index++;
                    }
                    if(pos->board[j][index_y] != ' ')
                    {
                        break;
                    }
                }
                //left
                for(int j = index_y-1; j >= 0; j--)
                {
                    if(pos->board[index_x][j] == ' ')
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*index_x+j;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(&all_moves[index], origin, index_x, j, ply, color);
                        index++;
                    }
                    else if(pos->board[index_x][j] == 'P' || pos->board[index_x][j] == 'R' || pos->board[index_x][j] == 'N' || pos->board[index_x][j] == 'B' ||
                            pos->board[index_x][j] == 'Q')
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*index_x+j;
                        all_moves[index].promotion = ' ';
                        sort[index] = bCapMove_score('r', pos->board[index_x][j], pos->board, index_x, index_y, index_x, j);
                        index++;
                    }
                    if(pos->board[index_x][j] != ' ')
                    {
                        break;
                    }
                }
                //right
                for(int j = index_y+1; j <= 7; j++)
                {
                    if(pos->board[index_x][j] == ' ')
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*index_x+j;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(&all_moves[index], origin, index_x, j, ply, color);
                        index++;
                    }
                    else if(pos->board[index_x][j] == 'P' || pos->board[index_x][j] == 'R' || pos->board[index_x][j] == 'N' || pos->board[index_x][j] == 'B' ||
                            pos->board[index_x][j] == 'Q')
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*index_x+j;
                        all_moves[index].promotion = ' ';
                        sort[index] = bCapMove_score('r', pos->board[index_x][j], pos->board, index_x, index_y, index_x, j);
                        index++;
                    }
                    if(pos->board[index_x][j] != ' ')
                    {
                        break;
                    }
                }
                //down
                for(int j = index_x+1; j <= 7; j++)
                {
                    if(pos->board[j][index_y] == ' ')
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*j+index_y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(&all_moves[index], origin, j, index_y, ply, color);
                        index++;
                    }
                    else if(pos->board[j][index_y] == 'P' || pos->board[j][index_y] == 'R' || pos->board[j][index_y] == 'N' || pos->board[j][index_y] == 'B' ||
                            pos->board[j][index_y] == 'Q')
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*j+index_y;
                        all_moves[index].promotion = ' ';
                        sort[index] = bCapMove_score('r', pos->board[j][index_y], pos->board, index_x, index_y, j, index_y);
                        index++;
                    }
                    if(pos->board[j][index_y] != ' ')
                    {
                        break;
                    }
                }
            }
            //bishop
            for(int i = 0; i < pos->piece_count[bB]; i++)
            {
                origin = pos->piece_list[bB][i];
                index_x = origin / 8;
                index_y = origin % 8;
                //up left
                for(int j = 1; j <= 7; j++)
                {
                    x = index_x - j;
                    y = index_y - j;
                    if(x < 0 || y < 0)
                    {
                        break;
                    }
                    if(pos->board[x][y] == ' ')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(&all_moves[index], origin, x, y, ply, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        all_moves[index].promotion = ' ';
                        sort[index] = bCapMove_score('b', pos->board[x][y], pos->board, index_x, index_y, x, y);
                        index++;
                    }
                    if(pos->board[x][y] != ' ')
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
                    if(pos->board[x][y] == ' ')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(&all_moves[index], origin, x, y, ply, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        all_moves[index].promotion = ' ';
                        sort[index] = bCapMove_score('b', pos->board[x][y], pos->board, index_x, index_y, x, y);
                        index++;
                    }
                    if(pos->board[x][y] != ' ')
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
                    if(pos->board[x][y] == ' ')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(&all_moves[index], origin, x, y, ply, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        all_moves[index].promotion = ' ';
                        sort[index] = bCapMove_score('b', pos->board[x][y], pos->board, index_x, index_y, x, y);
                        index++;
                    }
                    if(pos->board[x][y] != ' ')
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
                    if(pos->board[x][y] == ' ')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(&all_moves[index], origin, x, y, ply, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        all_moves[index].promotion = ' ';
                        sort[index] = bCapMove_score('b', pos->board[x][y], pos->board, index_x, index_y, x, y);
                        index++;
                    }
                    if(pos->board[x][y] != ' ')
                    {
                        break;
                    }
                }
            }
            //queen
            for(int i = 0; i < pos->piece_count[bQ]; i++)
            {
                origin = pos->piece_list[bQ][i];
                index_x = origin / 8;
                index_y = origin % 8;
                //up left
                for(int j = 1; j <= 7; j++)
                {
                    x = index_x - j;
                    y = index_y - j;
                    if(x < 0 || y < 0)
                    {
                        break;
                    }
                    if(pos->board[x][y] == ' ')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(&all_moves[index], origin, x, y, ply, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        all_moves[index].promotion = ' ';
                        sort[index] = bCapMove_score('q', pos->board[x][y], pos->board, index_x, index_y, x, y);
                        index++;
                    }
                    if(pos->board[x][y] != ' ')
                    {
                        break;
                    }
                }
                //up
                for(int j = index_x-1; j >= 0; j--)
                {
                    if(pos->board[j][index_y] == ' ')
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*j+index_y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(&all_moves[index], origin, j, index_y, ply, color);
                        index++;
                    }
                    else if(pos->board[j][index_y] == 'P' || pos->board[j][index_y] == 'R' || pos->board[j][index_y] == 'N' || pos->board[j][index_y] == 'B' ||
                            pos->board[j][index_y] == 'Q')
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*j+index_y;
                        all_moves[index].promotion = ' ';
                        sort[index] = bCapMove_score('q', pos->board[j][index_y], pos->board, index_x, index_y, j, index_y);
                        index++;
                    }
                    if(pos->board[j][index_y] != ' ')
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
                    if(pos->board[x][y] == ' ')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(&all_moves[index], origin, x, y, ply, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        all_moves[index].promotion = ' ';
                        sort[index] = bCapMove_score('q', pos->board[x][y], pos->board, index_x, index_y, x, y);
                        index++;
                    }
                    if(pos->board[x][y] != ' ')
                    {
                        break;
                    }
                }
                //left
                for(int j = index_y-1; j >= 0; j--)
                {
                    if(pos->board[index_x][j] == ' ')
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*index_x+j;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(&all_moves[index], origin, index_x, j, ply, color);
                        index++;
                    }
                    else if(pos->board[index_x][j] == 'P' || pos->board[index_x][j] == 'R' || pos->board[index_x][j] == 'N' || pos->board[index_x][j] == 'B' ||
                            pos->board[index_x][j] == 'Q')
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*index_x+j;
                        all_moves[index].promotion = ' ';
                        sort[index] = bCapMove_score('q', pos->board[index_x][j], pos->board, index_x, index_y, index_x, j);
                        index++;
                    }
                    if(pos->board[index_x][j] != ' ')
                    {
                        break;
                    }
                }
                //right
                for(int j = index_y+1; j <= 7; j++)
                {
                    if(pos->board[index_x][j] == ' ')
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*index_x+j;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(&all_moves[index], origin, index_x, j, ply, color);
                        index++;
                    }
                    else if(pos->board[index_x][j] == 'P' || pos->board[index_x][j] == 'R' || pos->board[index_x][j] == 'N' || pos->board[index_x][j] == 'B' ||
                            pos->board[index_x][j] == 'Q')
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*index_x+j;
                        all_moves[index].promotion = ' ';
                        sort[index] = bCapMove_score('q', pos->board[index_x][j], pos->board, index_x, index_y, index_x, j);
                        index++;
                    }
                    if(pos->board[index_x][j] != ' ')
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
                    if(pos->board[x][y] == ' ')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(&all_moves[index], origin, x, y, ply, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        all_moves[index].promotion = ' ';
                        sort[index] = bCapMove_score('q', pos->board[x][y], pos->board, index_x, index_y, x, y);
                        index++;
                    }
                    if(pos->board[x][y] != ' ')
                    {
                        break;
                    }
                }
                //down
                for(int j = index_x+1; j <= 7; j++)
                {
                    if(pos->board[j][index_y] == ' ')
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*j+index_y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(&all_moves[index], origin, j, index_y, ply, color);
                        index++;
                    }
                    else if(pos->board[j][index_y] == 'P' || pos->board[j][index_y] == 'R' || pos->board[j][index_y] == 'N' || pos->board[j][index_y] == 'B' ||
                            pos->board[j][index_y] == 'Q')
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*j+index_y;
                        all_moves[index].promotion = ' ';
                        sort[index] = bCapMove_score('q', pos->board[j][index_y], pos->board, index_x, index_y, j, index_y);
                        index++;
                    }
                    if(pos->board[j][index_y] != ' ')
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
                    if(pos->board[x][y] == ' ')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(&all_moves[index], origin, x, y, ply, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        all_moves[index].promotion = ' ';
                        sort[index] = bCapMove_score('q', pos->board[x][y], pos->board, index_x, index_y, x, y);
                        index++;
                    }
                    if(pos->board[x][y] != ' ')
                    {
                        break;
                    }
                }
            }
            break;
        }
        //white pieces
        case -1:
        {
            //pawn
            for(int i = 0; i < pos->piece_count[wP]; i++)
            {
                origin = pos->piece_list[wP][i];
                index_x = origin / 8;
                index_y = origin % 8;
                for(y = index_y - 1; y < index_y + 2; y+=2)
                {
                    if(y & 8) //skip when out of board
                    {
                        continue;
                    }
                    if(CheckCapture_wpawn(pos, index_x - 1, y)) 
                    { 
                        //promotions
                        if(index_x == 1)
                        {
                            all_moves[index].from = origin;
                            all_moves[index].to = 8*(index_x-1)+y;
                            all_moves[index].promotion = 'q';
                            sort[index] = wCapMove_score('P', pos->board[index_x-1][y], pos->board, index_x, index_y, index_x-1, y) + 500;
                            index++;
                            all_moves[index].from = origin;
                            all_moves[index].to = 8*(index_x-1)+y;
                            all_moves[index].promotion = 'r';
                            sort[index] = UNDERPROM;
                            index++;
                            all_moves[index].from = origin;
                            all_moves[index].to = 8*(index_x-1)+y;
                            all_moves[index].promotion = 'b';
                            sort[index] = UNDERPROM;
                            index++;
                            all_moves[index].from = origin;
                            all_moves[index].to = 8*(index_x-1)+y;
                            all_moves[index].promotion = 'n';
                            sort[index] = UNDERPROM;
                            index++;
                        }
                        else
                        {
                            all_moves[index].from = origin;
                            all_moves[index].to = 8*(index_x-1)+y;
                            all_moves[index].promotion = ' ';
                            sort[index] = wCapMove_score('P', pos->board[index_x-1][y], pos->board, index_x, index_y, index_x-1, y);
                            index++;
                        }
                    }
                }
                //1 step
                if(pos->board[index_x - 1][index_y] == ' ')
                {
                    //promotions
                    if(index_x == 1)
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*(index_x-1)+index_y;
                        all_moves[index].promotion = 'q';
                        sort[index] = PROMOTION;
                        index++;
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*(index_x-1)+index_y;
                        all_moves[index].promotion = 'r';
                        sort[index] = UNDERPROM;
                        index++;
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*(index_x-1)+index_y;
                        all_moves[index].promotion = 'b';
                        sort[index] = UNDERPROM;
                        index++;
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*(index_x-1)+index_y;
                        all_moves[index].promotion = 'n';
                        sort[index] = UNDERPROM;
                        index++;
                    }
                    else
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*(index_x-1)+index_y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(&all_moves[index], origin, index_x-1, index_y, ply, color);
                        index++;
                    }
                }
                //2 steps at starting position
                if(index_x == 6)
                {
                    if(pos->board[5][index_y] == ' ' && pos->board[4][index_y] == ' ')  
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 32+index_y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(&all_moves[index], origin, 4, index_y, ply, color);
                        index++;
                    }
                }
            }
            //knight
            for(int i = 0; i < pos->piece_count[wN]; i++)
            {
                origin = pos->piece_list[wN][i];
                index_x = origin / 8;
                index_y = origin % 8;
                for(int j = 0; j < 8; j++)
                {
                    x = index_x + knight_moves_x[j];
                    y = index_y + knight_moves_y[j];
                    if(x & 8 || y & 8) //skip when out of board
                    {
                        continue;
                    }
                    if(pos->board[x][y] == ' ')   
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(&all_moves[index], origin, x, y, ply, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')   
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        all_moves[index].promotion = ' ';
                        sort[index] = wCapMove_score('N', pos->board[x][y], pos->board, index_x, index_y, x, y);
                        index++;
                    }
                }
            }
            //king
            origin = pos->piece_list[wK][0];
            index_x = origin / 8;
            index_y = origin % 8;
            //castling
            if(index_x == 7 && index_y == 4)
            {
                if(CheckMove_wkingside(pos))  
                {  
                    all_moves[index].from = origin;
                    all_moves[index].to = 62;
                    all_moves[index].promotion = ' ';
                    sort[index] = CASTLING;
                    index++;
                }
                if(CheckMove_wqueenside(pos))  
                {  
                    all_moves[index].from = origin;
                    all_moves[index].to = 58;
                    all_moves[index].promotion = ' ';
                    sort[index] = CASTLING;
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
                if(pos->board[x][y] == ' ')   
                {  
                    all_moves[index].from = origin;
                    all_moves[index].to = 8*x+y;
                    all_moves[index].promotion = ' ';
                    sort[index] = quietMove_score(&all_moves[index], origin, x, y, ply, color);
                    index++;
                }
                else if(pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')   
                {  
                    all_moves[index].from = origin;
                    all_moves[index].to = 8*x+y;
                    all_moves[index].promotion = ' ';
                    sort[index] = wCapMove_score('K', pos->board[x][y], pos->board, index_x, index_y, x, y);
                    index++;
                }
            }
            //rook
            for(int i = 0; i < pos->piece_count[wR]; i++)
            {
                origin = pos->piece_list[wR][i];
                index_x = origin / 8;
                index_y = origin % 8;
                //up
                for(int j = index_x-1; j >= 0; j--)
                {
                    if(pos->board[j][index_y] == ' ') 
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*j+index_y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(&all_moves[index], origin, j, index_y, ply, color);
                        index++;
                    }
                    else if(pos->board[j][index_y] == 'p' || pos->board[j][index_y] == 'r' || pos->board[j][index_y] == 'n' || pos->board[j][index_y] == 'b' ||
                            pos->board[j][index_y] == 'q') 
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*j+index_y;
                        all_moves[index].promotion = ' ';
                        sort[index] = wCapMove_score('R', pos->board[j][index_y], pos->board, index_x, index_y, j, index_y);
                        index++;
                    }
                    if(pos->board[j][index_y] != ' ')
                    {
                        break;
                    }
                }
                //left
                for(int j = index_y-1; j >= 0; j--)
                {
                    if(pos->board[index_x][j] == ' ') 
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*index_x+j;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(&all_moves[index], origin, index_x, j, ply, color);
                        index++;
                    }
                    else if(pos->board[index_x][j] == 'p' || pos->board[index_x][j] == 'r' || pos->board[index_x][j] == 'n' || pos->board[index_x][j] == 'b' ||
                            pos->board[index_x][j] == 'q') 
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*index_x+j;
                        all_moves[index].promotion = ' ';
                        sort[index] = wCapMove_score('R', pos->board[index_x][j], pos->board, index_x, index_y, index_x, j);
                        index++;
                    }
                    if(pos->board[index_x][j] != ' ')
                    {
                        break;
                    }
                }
                //right
                for(int j = index_y+1; j <= 7; j++)
                {
                    if(pos->board[index_x][j] == ' ') 
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*index_x+j;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(&all_moves[index], origin, index_x, j, ply, color);
                        index++;
                    }
                    else if(pos->board[index_x][j] == 'p' || pos->board[index_x][j] == 'r' || pos->board[index_x][j] == 'n' || pos->board[index_x][j] == 'b' ||
                            pos->board[index_x][j] == 'q') 
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*index_x+j;
                        all_moves[index].promotion = ' ';
                        sort[index] = wCapMove_score('R', pos->board[index_x][j], pos->board, index_x, index_y, index_x, j);
                        index++;
                    }
                    if(pos->board[index_x][j] != ' ')
                    {
                        break;
                    }
                }
                //down
                for(int j = index_x+1; j <= 7; j++)
                {
                    if(pos->board[j][index_y] == ' ') 
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*j+index_y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(&all_moves[index], origin, j, index_y, ply, color);
                        index++;
                    }
                    else if(pos->board[j][index_y] == 'p' || pos->board[j][index_y] == 'r' || pos->board[j][index_y] == 'n' || pos->board[j][index_y] == 'b' ||
                            pos->board[j][index_y] == 'q') 
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*j+index_y;
                        all_moves[index].promotion = ' ';
                        sort[index] = wCapMove_score('R', pos->board[j][index_y], pos->board, index_x, index_y, j, index_y);
                        index++;
                    }
                    if(pos->board[j][index_y] != ' ')
                    {
                        break;
                    }
                }
            }
            //bishop
            for(int i = 0; i < pos->piece_count[wB]; i++)
            {
                origin = pos->piece_list[wB][i];
                index_x = origin / 8;
                index_y = origin % 8;
                //up left
                for(int j = 1; j <= 7; j++)
                {
                    x = index_x - j;
                    y = index_y - j;
                    if(x < 0 || y < 0)
                    {
                        break;
                    }
                    if(pos->board[x][y] == ' ')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(&all_moves[index], origin, x, y, ply, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        all_moves[index].promotion = ' ';
                        sort[index] = wCapMove_score('B', pos->board[x][y], pos->board, index_x, index_y, x, y);
                        index++;
                    }
                    if(pos->board[x][y] != ' ')
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
                    if(pos->board[x][y] == ' ')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(&all_moves[index], origin, x, y, ply, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        all_moves[index].promotion = ' ';
                        sort[index] = wCapMove_score('B', pos->board[x][y], pos->board, index_x, index_y, x, y);
                        index++;
                    }
                    if(pos->board[x][y] != ' ')
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
                    if(pos->board[x][y] == ' ')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(&all_moves[index], origin, x, y, ply, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        all_moves[index].promotion = ' ';
                        sort[index] = wCapMove_score('B', pos->board[x][y], pos->board, index_x, index_y, x, y);
                        index++;
                    }
                    if(pos->board[x][y] != ' ')
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
                    if(pos->board[x][y] == ' ')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(&all_moves[index], origin, x, y, ply, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        all_moves[index].promotion = ' ';
                        sort[index] = wCapMove_score('B', pos->board[x][y], pos->board, index_x, index_y, x, y);
                        index++;
                    }
                    if(pos->board[x][y] != ' ')
                    {
                        break;
                    }
                }
            }
            //queen
            for(int i = 0; i < pos->piece_count[wQ]; i++)
            {
                origin = pos->piece_list[wQ][i];
                index_x = origin / 8;
                index_y = origin % 8;
                //up left
                for(int j = 1; j <= 7; j++)
                {
                    x = index_x - j;
                    y = index_y - j;
                    if(x < 0 || y < 0)
                    {
                        break;
                    }
                    if(pos->board[x][y] == ' ')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(&all_moves[index], origin, x, y, ply, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        all_moves[index].promotion = ' ';
                        sort[index] = wCapMove_score('Q', pos->board[x][y], pos->board, index_x, index_y, x, y);
                        index++;
                    }
                    if(pos->board[x][y] != ' ')
                    {
                        break;
                    }
                }
                //up
                for(int j = index_x-1; j >= 0; j--)
                {
                    if(pos->board[j][index_y] == ' ') 
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*j+index_y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(&all_moves[index], origin, j, index_y, ply, color);
                        index++;
                    }
                    else if(pos->board[j][index_y] == 'p' || pos->board[j][index_y] == 'r' || pos->board[j][index_y] == 'n' || pos->board[j][index_y] == 'b' ||
                            pos->board[j][index_y] == 'q') 
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*j+index_y;
                        all_moves[index].promotion = ' ';
                        sort[index] = wCapMove_score('Q', pos->board[j][index_y], pos->board, index_x, index_y, j, index_y);
                        index++;
                    }
                    if(pos->board[j][index_y] != ' ')
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
                    if(pos->board[x][y] == ' ')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(&all_moves[index], origin, x, y, ply, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        all_moves[index].promotion = ' ';
                        sort[index] = wCapMove_score('Q', pos->board[x][y], pos->board, index_x, index_y, x, y);
                        index++;
                    }
                    if(pos->board[x][y] != ' ')
                    {
                        break;
                    }
                }
                //left
                for(int j = index_y-1; j >= 0; j--)
                {
                    if(pos->board[index_x][j] == ' ') 
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*index_x+j;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(&all_moves[index], origin, index_x, j, ply, color);
                        index++;
                    }
                    else if(pos->board[index_x][j] == 'p' || pos->board[index_x][j] == 'r' || pos->board[index_x][j] == 'n' || pos->board[index_x][j] == 'b' ||
                            pos->board[index_x][j] == 'q') 
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*index_x+j;
                        all_moves[index].promotion = ' ';
                        sort[index] = wCapMove_score('Q', pos->board[index_x][j], pos->board, index_x, index_y, index_x, j);
                        index++;
                    }
                    if(pos->board[index_x][j] != ' ')
                    {
                        break;
                    }
                }
                //right
                for(int j = index_y+1; j <= 7; j++)
                {
                    if(pos->board[index_x][j] == ' ') 
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*index_x+j;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(&all_moves[index], origin, index_x, j, ply, color);
                        index++;
                    }
                    else if(pos->board[index_x][j] == 'p' || pos->board[index_x][j] == 'r' || pos->board[index_x][j] == 'n' || pos->board[index_x][j] == 'b' ||
                            pos->board[index_x][j] == 'q') 
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*index_x+j;
                        all_moves[index].promotion = ' ';
                        sort[index] = wCapMove_score('Q', pos->board[index_x][j], pos->board, index_x, index_y, index_x, j);
                        index++;
                    }
                    if(pos->board[index_x][j] != ' ')
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
                    if(pos->board[x][y] == ' ')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(&all_moves[index], origin, x, y, ply, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        all_moves[index].promotion = ' ';
                        sort[index] = wCapMove_score('Q', pos->board[x][y], pos->board, index_x, index_y, x, y);
                        index++;
                    }
                    if(pos->board[x][y] != ' ')
                    {
                        break;
                    }
                }
                //down
                for(int j = index_x+1; j <= 7; j++)
                {
                    if(pos->board[j][index_y] == ' ') 
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*j+index_y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(&all_moves[index], origin, j, index_y, ply, color);
                        index++;
                    }
                    else if(pos->board[j][index_y] == 'p' || pos->board[j][index_y] == 'r' || pos->board[j][index_y] == 'n' || pos->board[j][index_y] == 'b' ||
                            pos->board[j][index_y] == 'q') 
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*j+index_y;
                        all_moves[index].promotion = ' ';
                        sort[index] = wCapMove_score('Q', pos->board[j][index_y], pos->board, index_x, index_y, j, index_y);
                        index++;
                    }
                    if(pos->board[j][index_y] != ' ')
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
                    if(pos->board[x][y] == ' ')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(&all_moves[index], origin, x, y, ply, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        all_moves[index].promotion = ' ';
                        sort[index] = wCapMove_score('Q', pos->board[x][y], pos->board, index_x, index_y, x, y);
                        index++;
                    }
                    if(pos->board[x][y] != ' ')
                    {
                        break;
                    }
                }
            }
            break;
        }
    }
    
    return index;
}