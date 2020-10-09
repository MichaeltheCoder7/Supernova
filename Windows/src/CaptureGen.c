#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>
#include "CaptureGen.h"
#include "MoveGen.h"
#include "CheckMove.h"
#include "OrderMove.h"
#include "Board.h"
#include "Move.h"

//generate caputures and queen promotion
int captureGen(BOARD *pos, MOVE all_moves[256], int sort[100], int color)
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
                if((index_y - 1) >= 0 && CheckCapture_bpawn(pos, index_x + 1, index_y - 1)) 
                { 
                    all_moves[index].from = origin;
                    all_moves[index].to = 8*(index_x+1)+index_y-1;
                    sort[index] = bCapQsearch_score('p', pos->board[index_x + 1][index_y - 1], index_x + 1);
                    index++;
                }
                //promotion
                if(index_x == 6 && pos->board[7][index_y] == ' ')
                {
                    all_moves[index].from = origin;
                    all_moves[index].to = 56+index_y;
                    sort[index] = 1000;
                    index++;
                } 
                if((index_y + 1) <= 7 && CheckCapture_bpawn(pos, index_x + 1, index_y + 1)) 
                { 
                    all_moves[index].from = origin;
                    all_moves[index].to = 8*(index_x+1)+index_y+1;
                    sort[index] = bCapQsearch_score('p', pos->board[index_x + 1][index_y + 1], index_x + 1);
                    index++;
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
                    if(pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q')
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        sort[index] = bCapQsearch_score('n', pos->board[x][y], 1);
                        index++;
                    }
                }
            }
            //king
            origin = pos->piece_list[bK][0];
            index_x = origin / 8;
            index_y = origin % 8;
            for(int j = 0; j < 8; j++)
            {
                x = index_x + king_moves_x[j];
                y = index_y + king_moves_y[j];
                if(x & 8 || y & 8) //skip when out of board
                {
                    continue;
                }
                if(pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q')  
                {  
                    all_moves[index].from = origin;
                    all_moves[index].to = 8*x+y;
                    sort[index] = bCapQsearch_score('k', pos->board[x][y], 1);
                    index++;
                }
            }
            //rook
            for(int i = 0; i < pos->piece_count[bR]; i++)
            {
                origin = pos->piece_list[bR][i];
                index_x = origin / 8;
                index_y = origin % 8;
                //up
                for(int j = index_x-1; j >= 0; j--)
                {  
                    if(pos->board[j][index_y] == 'P' || pos->board[j][index_y] == 'R' || pos->board[j][index_y] == 'N' || pos->board[j][index_y] == 'B' || pos->board[j][index_y] == 'Q')
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*j+index_y;
                        sort[index] = bCapQsearch_score('r', pos->board[j][index_y], 1);
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
                    if(pos->board[index_x][j] == 'P' || pos->board[index_x][j] == 'R' || pos->board[index_x][j] == 'N' || pos->board[index_x][j] == 'B' || pos->board[index_x][j] == 'Q')
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*index_x+j;
                        sort[index] = bCapQsearch_score('r', pos->board[index_x][j], 1);
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
                    if(pos->board[index_x][j] == 'P' || pos->board[index_x][j] == 'R' || pos->board[index_x][j] == 'N' || pos->board[index_x][j] == 'B' || pos->board[index_x][j] == 'Q')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*index_x+j;
                        sort[index] = bCapQsearch_score('r', pos->board[index_x][j], 1);
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
                    if(pos->board[j][index_y] == 'P' || pos->board[j][index_y] == 'R' || pos->board[j][index_y] == 'N' || pos->board[j][index_y] == 'B' || pos->board[j][index_y] == 'Q') 
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*j+index_y;
                        sort[index] = bCapQsearch_score('r', pos->board[j][index_y], 1);
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
                    if(pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        sort[index] = bCapQsearch_score('b', pos->board[x][y], 1);
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
                    if(pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        sort[index] = bCapQsearch_score('b', pos->board[x][y], 1);
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
                    if(pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        sort[index] = bCapQsearch_score('b', pos->board[x][y], 1);
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
                    if(pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        sort[index] = bCapQsearch_score('b', pos->board[x][y], 1);
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
                    if(pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        sort[index] = bCapQsearch_score('q', pos->board[x][y], 1);
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
                    if(pos->board[j][index_y] == 'P' || pos->board[j][index_y] == 'R' || pos->board[j][index_y] == 'N' || pos->board[j][index_y] == 'B' || pos->board[j][index_y] == 'Q') 
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*j+index_y;
                        sort[index] = bCapQsearch_score('q', pos->board[j][index_y], 1);
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
                    if(pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        sort[index] = bCapQsearch_score('q', pos->board[x][y], 1);
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
                    if(pos->board[index_x][j] == 'P' || pos->board[index_x][j] == 'R' || pos->board[index_x][j] == 'N' || pos->board[index_x][j] == 'B' || pos->board[index_x][j] == 'Q')
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*index_x+j;
                        sort[index] = bCapQsearch_score('q', pos->board[index_x][j], 1);
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
                    if(pos->board[index_x][j] == 'P' || pos->board[index_x][j] == 'R' || pos->board[index_x][j] == 'N' || pos->board[index_x][j] == 'B' || pos->board[index_x][j] == 'Q') 
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*index_x+j;
                        sort[index] = bCapQsearch_score('q', pos->board[index_x][j], 1);
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
                    if(pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        sort[index] = bCapQsearch_score('q', pos->board[x][y], 1);
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
                    if(pos->board[j][index_y] == 'P' || pos->board[j][index_y] == 'R' || pos->board[j][index_y] == 'N' || pos->board[j][index_y] == 'B' || pos->board[j][index_y] == 'Q') 
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*j+index_y;
                        sort[index] = bCapQsearch_score('q', pos->board[j][index_y], 1);
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
                    if(pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        sort[index] = bCapQsearch_score('q', pos->board[x][y], 1);
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
                if((index_y - 1) >= 0 && CheckCapture_wpawn(pos, index_x - 1, index_y - 1)) 
                { 
                    all_moves[index].from = origin;
                    all_moves[index].to = 8*(index_x-1)+index_y-1;
                    sort[index] = wCapQsearch_score('P', pos->board[index_x - 1][index_y - 1], index_x - 1);
                    index++;
                }
                //promotion
                if(index_x == 1 && pos->board[0][index_y] == ' ')
                {
                    all_moves[index].from = origin;
                    all_moves[index].to = index_y;
                    sort[index] = 1000;
                    index++;
                } 
                if((index_y + 1) <= 7 && CheckCapture_wpawn(pos, index_x - 1, index_y + 1)) 
                { 
                    all_moves[index].from = origin;
                    all_moves[index].to = 8*(index_x-1)+index_y+1;
                    sort[index] = wCapQsearch_score('P', pos->board[index_x - 1][index_y + 1], index_x - 1);
                    index++;
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
                    if(pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        sort[index] = wCapQsearch_score('N', pos->board[x][y], 1);
                        index++;
                    }
                }
            }
            //king
            origin = pos->piece_list[wK][0];
            index_x = origin / 8;
            index_y = origin % 8;
            for(int j = 0; j < 8; j++)
            {
                x = index_x + king_moves_x[j];
                y = index_y + king_moves_y[j];
                if(x & 8 || y & 8) //skip when out of board
                {
                    continue;
                }
                if(pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')
                {  
                    all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                    sort[index] = wCapQsearch_score('K', pos->board[x][y], 1);
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
                    if(pos->board[j][index_y] == 'p' || pos->board[j][index_y] == 'r' || pos->board[j][index_y] == 'n' || pos->board[j][index_y] == 'b' || pos->board[j][index_y] == 'q') 
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*j+index_y;
                        sort[index] = wCapQsearch_score('R', pos->board[j][index_y], 1);
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
                    if(pos->board[index_x][j] == 'p' || pos->board[index_x][j] == 'r' || pos->board[index_x][j] == 'n' || pos->board[index_x][j] == 'b' || pos->board[index_x][j] == 'q') 
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*index_x+j;
                        sort[index] = wCapQsearch_score('R', pos->board[index_x][j], 1);
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
                    if(pos->board[index_x][j] == 'p' || pos->board[index_x][j] == 'r' || pos->board[index_x][j] == 'n' || pos->board[index_x][j] == 'b' || pos->board[index_x][j] == 'q')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*index_x+j;
                        sort[index] = wCapQsearch_score('R', pos->board[index_x][j], 1);
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
                    if(pos->board[j][index_y] == 'p' || pos->board[j][index_y] == 'r' || pos->board[j][index_y] == 'n' || pos->board[j][index_y] == 'b' || pos->board[j][index_y] == 'q') 
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*j+index_y;
                        sort[index] = wCapQsearch_score('R', pos->board[j][index_y], 1);
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
                    if(pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        sort[index] = wCapQsearch_score('B', pos->board[x][y], 1);
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
                    if(pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        sort[index] = wCapQsearch_score('B', pos->board[x][y], 1);
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
                    if(pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        sort[index] = wCapQsearch_score('B', pos->board[x][y], 1);
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
                    if(pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        sort[index] = wCapQsearch_score('B', pos->board[x][y], 1);
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
                index_x = pos->piece_list[wQ][i] / 8;
                index_y = pos->piece_list[wQ][i] % 8;
                //up left
                for(int j = 1; j <= 7; j++)
                {
                    x = index_x - j;
                    y = index_y - j;
                    if(x < 0 || y < 0)
                    {
                        break;
                    }
                    if(pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        sort[index] = wCapQsearch_score('Q', pos->board[x][y], 1);
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
                    if(pos->board[j][index_y] == 'p' || pos->board[j][index_y] == 'r' || pos->board[j][index_y] == 'n' || pos->board[j][index_y] == 'b' || pos->board[j][index_y] == 'q')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*j+index_y;
                        sort[index] = wCapQsearch_score('Q', pos->board[j][index_y], 1);
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
                    if(pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        sort[index] = wCapQsearch_score('Q', pos->board[x][y], 1);
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
                    if(pos->board[index_x][j] == 'p' || pos->board[index_x][j] == 'r' || pos->board[index_x][j] == 'n' || pos->board[index_x][j] == 'b' || pos->board[index_x][j] == 'q')   
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*index_x+j;
                        sort[index] = wCapQsearch_score('Q', pos->board[index_x][j], 1);
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
                    if(pos->board[index_x][j] == 'p' || pos->board[index_x][j] == 'r' || pos->board[index_x][j] == 'n' || pos->board[index_x][j] == 'b' || pos->board[index_x][j] == 'q')   
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*index_x+j;
                        sort[index] = wCapQsearch_score('Q', pos->board[index_x][j], 1);
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
                    if(pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        sort[index] = wCapQsearch_score('Q', pos->board[x][y], 1);
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
                    if(pos->board[j][index_y] == 'p' || pos->board[j][index_y] == 'r' || pos->board[j][index_y] == 'n' || pos->board[j][index_y] == 'b' || pos->board[j][index_y] == 'q') 
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*j+index_y;
                        sort[index] = wCapQsearch_score('Q', pos->board[j][index_y], 1);
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
                    if(pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')  
                    {  
                        all_moves[index].from = origin;
                        all_moves[index].to = 8*x+y;
                        sort[index] = wCapQsearch_score('Q', pos->board[x][y], 1);
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