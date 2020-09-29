#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>
#include "MoveGen.h"
#include "Attack.h"
#include "CheckMove.h"
#include "OrderMove.h"
#include "Board.h"

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

//generate all pseudo-legal moves at root
int moveGen_root(BOARD *pos, char all_moves[256][6], int sort[256], char best_move[6], char hash_move[6], int color)
{
    int index = 0; 
    int index_x;
    int index_y;
    int x, y;

    switch(color)
    {
        //black pieces
        case 1:
        {
            //pawn
            for(int i = 0; i < pos->piece_count[bP]; i++)
            {
                index_x = pos->piece_list[bP][i] / 8;
                index_y = pos->piece_list[bP][i] % 8;
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
                            strncpy(all_moves[index], positions[index_x][index_y], 3);
                            strncat(all_moves[index], positions[index_x + 1][y], 3);
                            all_moves[index][4] = 'q';
                            all_moves[index][5] = '\0';
                            sort[index] = bCapMove_score_root(all_moves[index], best_move, hash_move, 'p', pos->board[index_x + 1][y]);
                            index++;
                            strncpy(all_moves[index], all_moves[index - 1], 6);
                            all_moves[index][4] = 'r';
                            sort[index] = bCapMove_score_root(all_moves[index], best_move, hash_move, 'p', pos->board[index_x + 1][y]);
                            index++;
                            strncpy(all_moves[index], all_moves[index - 1], 6);
                            all_moves[index][4] = 'b';
                            sort[index] = bCapMove_score_root(all_moves[index], best_move, hash_move, 'p', pos->board[index_x + 1][y]);
                            index++;
                            strncpy(all_moves[index], all_moves[index - 1], 6);
                            all_moves[index][4] = 'n';
                            sort[index] = bCapMove_score_root(all_moves[index], best_move, hash_move, 'p', pos->board[index_x + 1][y]);
                            index++;
                        }
                        else
                        {
                            strncpy(all_moves[index], positions[index_x][index_y], 3);
                            strncat(all_moves[index], positions[index_x + 1][y], 3);
                            sort[index] = bCapMove_score_root(all_moves[index], best_move, hash_move, 'p', pos->board[index_x + 1][y]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x + 1][index_y], 3);
                        all_moves[index][4] = 'q';
                        all_moves[index][5] = '\0';
                        sort[index] = promotion_score_root(all_moves[index], best_move, hash_move);
                        index++;
                        strncpy(all_moves[index], all_moves[index - 1], 6);
                        all_moves[index][4] = 'r';
                        sort[index] = underpromotion_score_root(all_moves[index], best_move, hash_move);
                        index++;
                        strncpy(all_moves[index], all_moves[index - 1], 6);
                        all_moves[index][4] = 'b';
                        sort[index] = underpromotion_score_root(all_moves[index], best_move, hash_move);
                        index++;
                        strncpy(all_moves[index], all_moves[index - 1], 6);
                        all_moves[index][4] = 'n';
                        sort[index] = underpromotion_score_root(all_moves[index], best_move, hash_move);
                        index++;
                    }
                    else
                    {
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x + 1][index_y], 3);
                        sort[index] = quietMove_score_root(all_moves[index], best_move, hash_move, index_x, index_y, index_x + 1, index_y, color);
                        index++;
                    }
                }
                //2 steps at starting position
                if(index_x == 1)
                {
                    if(pos->board[2][index_y] == ' ' && pos->board[3][index_y] == ' ')  
                    { 
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[3][index_y], 3);
                        sort[index] = quietMove_score_root(all_moves[index], best_move, hash_move, index_x, index_y, 3, index_y, color);
                        index++;
                    }
                }
            }
            //knight
            for(int i = 0; i < pos->piece_count[bN]; i++)
            {
                index_x = pos->piece_list[bN][i] / 8;
                index_y = pos->piece_list[bN][i] % 8;
                for(int j = 0; j < 8; j++)
                {
                    x = index_x + knight_moves_x[j];
                    y = index_y + knight_moves_y[j];
                    if(x & 8 || y & 8) //skips when out of board
                    {
                        continue;
                    }
                    if(pos->board[x][y] == ' ')  
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = quietMove_score_root(all_moves[index], best_move, hash_move, index_x, index_y, x, y, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q')  
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = bCapMove_score_root(all_moves[index], best_move, hash_move, 'n', pos->board[x][y]);
                        index++;
                    }
                }
            }
            //king
            index_x = pos->piece_list[bK][0] / 8;
            index_y = pos->piece_list[bK][0] % 8;
            //castling
            if(index_x == 0 && index_y == 4)
            {
                if(CheckMove_bkingside(pos))  
                {  
                    strncpy(all_moves[index], positions[index_x][index_y], 3);
                    strncat(all_moves[index], positions[0][6], 3);
                    sort[index] = castling_score_root(all_moves[index], best_move, hash_move);
                    index++;
                }
                if(CheckMove_bqueenside(pos))  
                {  
                    strncpy(all_moves[index], positions[index_x][index_y], 3);
                    strncat(all_moves[index], positions[0][2], 3);
                    sort[index] = castling_score_root(all_moves[index], best_move, hash_move);
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
                    strncpy(all_moves[index], positions[index_x][index_y], 3);
                    strncat(all_moves[index], positions[x][y], 3);
                    sort[index] = quietMove_score_root(all_moves[index], best_move, hash_move, index_x, index_y, x, y, color);
                    index++;
                }
                else if(pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q') 
                {  
                    strncpy(all_moves[index], positions[index_x][index_y], 3);
                    strncat(all_moves[index], positions[x][y], 3);
                    sort[index] = bCapMove_score_root(all_moves[index], best_move, hash_move, 'k', pos->board[x][y]);
                    index++;
                }
            }
            //rook
            for(int i = 0; i < pos->piece_count[bR]; i++)
            {
                index_x = pos->piece_list[bR][i] / 8;
                index_y = pos->piece_list[bR][i] % 8;
                //up
                for(int j = index_x-1; j >= 0; j--)
                {  
                    if(pos->board[j][index_y] == ' ')
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
                        sort[index] = quietMove_score_root(all_moves[index], best_move, hash_move, index_x, index_y, j, index_y, color);
                        index++;
                    }
                    else if(pos->board[j][index_y] == 'P' || pos->board[j][index_y] == 'R' || pos->board[j][index_y] == 'N' || pos->board[j][index_y] == 'B' || pos->board[j][index_y] == 'Q')
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
                        sort[index] = bCapMove_score_root(all_moves[index], best_move, hash_move, 'r', pos->board[j][index_y]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
                        sort[index] = quietMove_score_root(all_moves[index], best_move, hash_move, index_x, index_y, index_x, j, color);
                        index++;
                    }
                    else if(pos->board[index_x][j] == 'P' || pos->board[index_x][j] == 'R' || pos->board[index_x][j] == 'N' || pos->board[index_x][j] == 'B' || pos->board[index_x][j] == 'Q')
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
                        sort[index] = bCapMove_score_root(all_moves[index], best_move, hash_move, 'r', pos->board[index_x][j]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
                        sort[index] = quietMove_score_root(all_moves[index], best_move, hash_move, index_x, index_y, index_x, j, color);
                        index++;
                    }
                    else if(pos->board[index_x][j] == 'P' || pos->board[index_x][j] == 'R' || pos->board[index_x][j] == 'N' || pos->board[index_x][j] == 'B' || pos->board[index_x][j] == 'Q')
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
                        sort[index] = bCapMove_score_root(all_moves[index], best_move, hash_move, 'r', pos->board[index_x][j]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
                        sort[index] = quietMove_score_root(all_moves[index], best_move, hash_move, index_x, index_y, j, index_y, color);
                        index++;
                    }
                    else if(pos->board[j][index_y] == 'P' || pos->board[j][index_y] == 'R' || pos->board[j][index_y] == 'N' || pos->board[j][index_y] == 'B' || pos->board[j][index_y] == 'Q')
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
                        sort[index] = bCapMove_score_root(all_moves[index], best_move, hash_move, 'r', pos->board[j][index_y]);
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
                index_x = pos->piece_list[bB][i] / 8;
                index_y = pos->piece_list[bB][i] % 8;
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = quietMove_score_root(all_moves[index], best_move, hash_move, index_x, index_y, x, y, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q')  
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = bCapMove_score_root(all_moves[index], best_move, hash_move, 'b', pos->board[x][y]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = quietMove_score_root(all_moves[index], best_move, hash_move, index_x, index_y, x, y, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q')  
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = bCapMove_score_root(all_moves[index], best_move, hash_move, 'b', pos->board[x][y]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = quietMove_score_root(all_moves[index], best_move, hash_move, index_x, index_y, x, y, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q')  
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = bCapMove_score_root(all_moves[index], best_move, hash_move, 'b', pos->board[x][y]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = quietMove_score_root(all_moves[index], best_move, hash_move, index_x, index_y, x, y, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q')  
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = bCapMove_score_root(all_moves[index], best_move, hash_move, 'b', pos->board[x][y]);
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
                index_x = pos->piece_list[bQ][i] / 8;
                index_y = pos->piece_list[bQ][i] % 8;
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = quietMove_score_root(all_moves[index], best_move, hash_move, index_x, index_y, x, y, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q')  
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = bCapMove_score_root(all_moves[index], best_move, hash_move, 'q', pos->board[x][y]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
                        sort[index] = quietMove_score_root(all_moves[index], best_move, hash_move, index_x, index_y, j, index_y, color);
                        index++;
                    }
                    else if(pos->board[j][index_y] == 'P' || pos->board[j][index_y] == 'R' || pos->board[j][index_y] == 'N' || pos->board[j][index_y] == 'B' || pos->board[j][index_y] == 'Q')
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
                        sort[index] = bCapMove_score_root(all_moves[index], best_move, hash_move, 'q', pos->board[j][index_y]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = quietMove_score_root(all_moves[index], best_move, hash_move, index_x, index_y, x, y, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q')  
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = bCapMove_score_root(all_moves[index], best_move, hash_move, 'q', pos->board[x][y]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
                        sort[index] = quietMove_score_root(all_moves[index], best_move, hash_move, index_x, index_y, index_x, j, color);
                        index++;
                    }
                    else if(pos->board[index_x][j] == 'P' || pos->board[index_x][j] == 'R' || pos->board[index_x][j] == 'N' || pos->board[index_x][j] == 'B' || pos->board[index_x][j] == 'Q')
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
                        sort[index] = bCapMove_score_root(all_moves[index], best_move, hash_move, 'q', pos->board[index_x][j]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
                        sort[index] = quietMove_score_root(all_moves[index], best_move, hash_move, index_x, index_y, index_x, j, color);
                        index++;
                    }
                    else if(pos->board[index_x][j] == 'P' || pos->board[index_x][j] == 'R' || pos->board[index_x][j] == 'N' || pos->board[index_x][j] == 'B' || pos->board[index_x][j] == 'Q')
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
                        sort[index] = bCapMove_score_root(all_moves[index], best_move, hash_move, 'q', pos->board[index_x][j]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = quietMove_score_root(all_moves[index], best_move, hash_move, index_x, index_y, x, y, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q')  
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = bCapMove_score_root(all_moves[index], best_move, hash_move, 'q', pos->board[x][y]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
                        sort[index] = quietMove_score_root(all_moves[index], best_move, hash_move, index_x, index_y, j, index_y, color);
                        index++;
                    }
                    else if(pos->board[j][index_y] == 'P' || pos->board[j][index_y] == 'R' || pos->board[j][index_y] == 'N' || pos->board[j][index_y] == 'B' || pos->board[j][index_y] == 'Q')
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
                        sort[index] = bCapMove_score_root(all_moves[index], best_move, hash_move, 'q', pos->board[j][index_y]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = quietMove_score_root(all_moves[index], best_move, hash_move, index_x, index_y, x, y, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q')  
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = bCapMove_score_root(all_moves[index], best_move, hash_move, 'q', pos->board[x][y]);
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
                index_x = pos->piece_list[wP][i] / 8;
                index_y = pos->piece_list[wP][i] % 8;
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
                            strncpy(all_moves[index], positions[index_x][index_y], 3);
                            strncat(all_moves[index], positions[index_x - 1][y], 3);
                            all_moves[index][4] = 'q';
                            all_moves[index][5] = '\0';
                            sort[index] = wCapMove_score_root(all_moves[index], best_move, hash_move, 'P', pos->board[index_x - 1][y]);
                            index++;
                            strncpy(all_moves[index], all_moves[index - 1], 6);
                            all_moves[index][4] = 'r';
                            sort[index] = wCapMove_score_root(all_moves[index], best_move, hash_move, 'P', pos->board[index_x - 1][y]);
                            index++;
                            strncpy(all_moves[index], all_moves[index - 1], 6);
                            all_moves[index][4] = 'b';
                            sort[index] = wCapMove_score_root(all_moves[index], best_move, hash_move, 'P', pos->board[index_x - 1][y]);
                            index++;
                            strncpy(all_moves[index], all_moves[index - 1], 6);
                            all_moves[index][4] = 'n';
                            sort[index] = wCapMove_score_root(all_moves[index], best_move, hash_move, 'P', pos->board[index_x - 1][y]);
                            index++;
                        }
                        else
                        {
                            strncpy(all_moves[index], positions[index_x][index_y], 3);
                            strncat(all_moves[index], positions[index_x - 1][y], 3);
                            sort[index] = wCapMove_score_root(all_moves[index], best_move, hash_move, 'P', pos->board[index_x - 1][y]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x - 1][index_y], 3);
                        all_moves[index][4] = 'q';
                        all_moves[index][5] = '\0';
                        sort[index] = promotion_score_root(all_moves[index], best_move, hash_move);
                        index++;
                        strncpy(all_moves[index], all_moves[index - 1], 6);
                        all_moves[index][4] = 'r';
                        sort[index] = underpromotion_score_root(all_moves[index], best_move, hash_move);
                        index++;
                        strncpy(all_moves[index], all_moves[index - 1], 6);
                        all_moves[index][4] = 'b';
                        sort[index] = underpromotion_score_root(all_moves[index], best_move, hash_move);
                        index++;
                        strncpy(all_moves[index], all_moves[index - 1], 6);
                        all_moves[index][4] = 'n';
                        sort[index] = underpromotion_score_root(all_moves[index], best_move, hash_move);
                        index++;
                    }
                    else
                    {
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x - 1][index_y], 3);
                        sort[index] = quietMove_score_root(all_moves[index], best_move, hash_move, index_x, index_y, index_x - 1, index_y, color);
                        index++;
                    }
                }
                //2 steps at starting position
                if(index_x == 6)
                {
                    if(pos->board[5][index_y] == ' ' && pos->board[4][index_y] == ' ')  
                    { 
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[4][index_y], 3);
                        sort[index] = quietMove_score_root(all_moves[index], best_move, hash_move, index_x, index_y, 4, index_y, color);
                        index++;
                    }
                }
            }
            //knight
            for(int i = 0; i < pos->piece_count[wN]; i++)
            {
                index_x = pos->piece_list[wN][i] / 8;
                index_y = pos->piece_list[wN][i] % 8;
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = quietMove_score_root(all_moves[index], best_move, hash_move, index_x, index_y, x, y, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')   
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = wCapMove_score_root(all_moves[index], best_move, hash_move, 'N', pos->board[x][y]);
                        index++;
                    }
                }
            }
            //king
            index_x = pos->piece_list[wK][0] / 8;
            index_y = pos->piece_list[wK][0] % 8;
            //castling
            if(index_x == 7 && index_y == 4)
            {
                if(CheckMove_wkingside(pos))  
                {  
                    strncpy(all_moves[index], positions[index_x][index_y], 3);
                    strncat(all_moves[index], positions[7][6], 3);
                    sort[index] = castling_score_root(all_moves[index], best_move, hash_move);
                    index++;
                }
                if(CheckMove_wqueenside(pos))  
                {  
                    strncpy(all_moves[index], positions[index_x][index_y], 3);
                    strncat(all_moves[index], positions[7][2], 3);
                    sort[index] = castling_score_root(all_moves[index], best_move, hash_move);
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
                    strncpy(all_moves[index], positions[index_x][index_y], 3);
                    strncat(all_moves[index], positions[x][y], 3);
                    sort[index] = quietMove_score_root(all_moves[index], best_move, hash_move, index_x, index_y, x, y, color);
                    index++;
                }
                else if(pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')   
                {  
                    strncpy(all_moves[index], positions[index_x][index_y], 3);
                    strncat(all_moves[index], positions[x][y], 3);
                    sort[index] = wCapMove_score_root(all_moves[index], best_move, hash_move, 'K', pos->board[x][y]);
                    index++;
                }
            }
            //rook
            for(int i = 0; i < pos->piece_count[wR]; i++)
            {
                index_x = pos->piece_list[wR][i] / 8;
                index_y = pos->piece_list[wR][i] % 8;
                //up
                for(int j = index_x-1; j >= 0; j--)
                {
                    if(pos->board[j][index_y] == ' ') 
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
                        sort[index] = quietMove_score_root(all_moves[index], best_move, hash_move, index_x, index_y, j, index_y, color);
                        index++;
                    }
                    else if(pos->board[j][index_y] == 'p' || pos->board[j][index_y] == 'r' || pos->board[j][index_y] == 'n' || pos->board[j][index_y] == 'b' || pos->board[j][index_y] == 'q') 
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
                        sort[index] = wCapMove_score_root(all_moves[index], best_move, hash_move, 'R', pos->board[j][index_y]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
                        sort[index] = quietMove_score_root(all_moves[index], best_move, hash_move, index_x, index_y, index_x, j, color);
                        index++;
                    }
                    else if(pos->board[index_x][j] == 'p' || pos->board[index_x][j] == 'r' || pos->board[index_x][j] == 'n' || pos->board[index_x][j] == 'b' || pos->board[index_x][j] == 'q') 
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
                        sort[index] = wCapMove_score_root(all_moves[index], best_move, hash_move, 'R', pos->board[index_x][j]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
                        sort[index] = quietMove_score_root(all_moves[index], best_move, hash_move, index_x, index_y, index_x, j, color);
                        index++;
                    }
                    else if(pos->board[index_x][j] == 'p' || pos->board[index_x][j] == 'r' || pos->board[index_x][j] == 'n' || pos->board[index_x][j] == 'b' || pos->board[index_x][j] == 'q') 
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
                        sort[index] = wCapMove_score_root(all_moves[index], best_move, hash_move, 'R', pos->board[index_x][j]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
                        sort[index] = quietMove_score_root(all_moves[index], best_move, hash_move, index_x, index_y, j, index_y, color);
                        index++;
                    }
                    else if(pos->board[j][index_y] == 'p' || pos->board[j][index_y] == 'r' || pos->board[j][index_y] == 'n' || pos->board[j][index_y] == 'b' || pos->board[j][index_y] == 'q') 
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
                        sort[index] = wCapMove_score_root(all_moves[index], best_move, hash_move, 'R', pos->board[j][index_y]);
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
                index_x = pos->piece_list[wB][i] / 8;
                index_y = pos->piece_list[wB][i] % 8;
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = quietMove_score_root(all_moves[index], best_move, hash_move, index_x, index_y, x, y, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')  
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = wCapMove_score_root(all_moves[index], best_move, hash_move, 'B', pos->board[x][y]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = quietMove_score_root(all_moves[index], best_move, hash_move, index_x, index_y, x, y, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')  
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = wCapMove_score_root(all_moves[index], best_move, hash_move, 'B', pos->board[x][y]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = quietMove_score_root(all_moves[index], best_move, hash_move, index_x, index_y, x, y, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')  
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = wCapMove_score_root(all_moves[index], best_move, hash_move, 'B', pos->board[x][y]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = quietMove_score_root(all_moves[index], best_move, hash_move, index_x, index_y, x, y, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')  
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = wCapMove_score_root(all_moves[index], best_move, hash_move, 'B', pos->board[x][y]);
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
                    if(pos->board[x][y] == ' ')  
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = quietMove_score_root(all_moves[index], best_move, hash_move, index_x, index_y, x, y, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')  
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = wCapMove_score_root(all_moves[index], best_move, hash_move, 'Q', pos->board[x][y]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
                        sort[index] = quietMove_score_root(all_moves[index], best_move, hash_move, index_x, index_y, j, index_y, color);
                        index++;
                    }
                    else if(pos->board[j][index_y] == 'p' || pos->board[j][index_y] == 'r' || pos->board[j][index_y] == 'n' || pos->board[j][index_y] == 'b' || pos->board[j][index_y] == 'q') 
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
                        sort[index] = wCapMove_score_root(all_moves[index], best_move, hash_move, 'Q', pos->board[j][index_y]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = quietMove_score_root(all_moves[index], best_move, hash_move, index_x, index_y, x, y, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')  
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = wCapMove_score_root(all_moves[index], best_move, hash_move, 'Q', pos->board[x][y]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
                        sort[index] = quietMove_score_root(all_moves[index], best_move, hash_move, index_x, index_y, index_x, j, color);
                        index++;
                    }
                    else if(pos->board[index_x][j] == 'p' || pos->board[index_x][j] == 'r' || pos->board[index_x][j] == 'n' || pos->board[index_x][j] == 'b' || pos->board[index_x][j] == 'q') 
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
                        sort[index] = wCapMove_score_root(all_moves[index], best_move, hash_move, 'Q', pos->board[index_x][j]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
                        sort[index] = quietMove_score_root(all_moves[index], best_move, hash_move, index_x, index_y, index_x, j, color);
                        index++;
                    }
                    else if(pos->board[index_x][j] == 'p' || pos->board[index_x][j] == 'r' || pos->board[index_x][j] == 'n' || pos->board[index_x][j] == 'b' || pos->board[index_x][j] == 'q') 
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
                        sort[index] = wCapMove_score_root(all_moves[index], best_move, hash_move, 'Q', pos->board[index_x][j]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = quietMove_score_root(all_moves[index], best_move, hash_move, index_x, index_y, x, y, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')  
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = wCapMove_score_root(all_moves[index], best_move, hash_move, 'Q', pos->board[x][y]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
                        sort[index] = quietMove_score_root(all_moves[index], best_move, hash_move, index_x, index_y, j, index_y, color);
                        index++;
                    }
                    else if(pos->board[j][index_y] == 'p' || pos->board[j][index_y] == 'r' || pos->board[j][index_y] == 'n' || pos->board[j][index_y] == 'b' || pos->board[j][index_y] == 'q') 
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
                        sort[index] = wCapMove_score_root(all_moves[index], best_move, hash_move, 'Q', pos->board[j][index_y]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = quietMove_score_root(all_moves[index], best_move, hash_move, index_x, index_y, x, y, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')  
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = wCapMove_score_root(all_moves[index], best_move, hash_move, 'Q', pos->board[x][y]);
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

//generate all pseudo-legal moves
//don't generate under promotions to bishop and rook
int moveGen(BOARD *pos, char all_moves[256][6], int sort[256], char hash_move[6], int ply, int color)
{
    int index = 0; 
    int index_x;
    int index_y;
    int x, y;

    switch(color)
    {
        //black pieces
        case 1:
        {
            //pawn
            for(int i = 0; i < pos->piece_count[bP]; i++)
            {
                index_x = pos->piece_list[bP][i] / 8;
                index_y = pos->piece_list[bP][i] % 8;
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
                            strncpy(all_moves[index], positions[index_x][index_y], 3);
                            strncat(all_moves[index], positions[index_x + 1][y], 3);
                            all_moves[index][4] = 'q';
                            all_moves[index][5] = '\0';
                            sort[index] = bCapMove_score(all_moves[index], hash_move, 'p', pos->board[index_x + 1][y]);
                            index++;
                            strncpy(all_moves[index], all_moves[index - 1], 6);
                            all_moves[index][4] = 'n';
                            sort[index] = bCapMove_score(all_moves[index], hash_move, 'p', pos->board[index_x + 1][y]);
                            index++;
                        }
                        else
                        {
                            strncpy(all_moves[index], positions[index_x][index_y], 3);
                            strncat(all_moves[index], positions[index_x + 1][y], 3);
                            sort[index] = bCapMove_score(all_moves[index], hash_move, 'p', pos->board[index_x + 1][y]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x + 1][index_y], 3);
                        all_moves[index][4] = 'q';
                        all_moves[index][5] = '\0';
                        sort[index] = promotion_score(all_moves[index], hash_move);
                        index++;
                        strncpy(all_moves[index], all_moves[index - 1], 6);
                        all_moves[index][4] = 'n';
                        sort[index] = underpromotion_score(all_moves[index], hash_move);
                        index++;
                    }
                    else
                    {
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x + 1][index_y], 3);
                        sort[index] = quietMove_score(all_moves[index], hash_move, index_x, index_y, index_x + 1, index_y, ply, color);
                        index++;
                    }
                }
                //2 steps at starting position
                if(index_x == 1)
                {
                    if(pos->board[2][index_y] == ' ' && pos->board[3][index_y] == ' ')  
                    { 
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[3][index_y], 3);
                        sort[index] = quietMove_score(all_moves[index], hash_move, index_x, index_y, 3, index_y, ply, color);
                        index++;
                    }
                }
            }
            //knight
            for(int i = 0; i < pos->piece_count[bN]; i++)
            {
                index_x = pos->piece_list[bN][i] / 8;
                index_y = pos->piece_list[bN][i] % 8;
                for(int j = 0; j < 8; j++)
                {
                    x = index_x + knight_moves_x[j];
                    y = index_y + knight_moves_y[j];
                    if(x & 8 || y & 8) //skips when out of board
                    {
                        continue;
                    }
                    if(pos->board[x][y] == ' ')  
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = quietMove_score(all_moves[index], hash_move, index_x, index_y, x, y, ply, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q')  
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = bCapMove_score(all_moves[index], hash_move, 'n', pos->board[x][y]);
                        index++;
                    }
                }
            }
            //king
            index_x = pos->piece_list[bK][0] / 8;
            index_y = pos->piece_list[bK][0] % 8;
            //castling
            if(index_x == 0 && index_y == 4)
            {
                if(CheckMove_bkingside(pos))  
                {  
                    strncpy(all_moves[index], positions[index_x][index_y], 3);
                    strncat(all_moves[index], positions[0][6], 3);
                    sort[index] = castling_score(all_moves[index], hash_move);
                    index++;
                }
                if(CheckMove_bqueenside(pos))  
                {  
                    strncpy(all_moves[index], positions[index_x][index_y], 3);
                    strncat(all_moves[index], positions[0][2], 3);
                    sort[index] = castling_score(all_moves[index], hash_move);
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
                    strncpy(all_moves[index], positions[index_x][index_y], 3);
                    strncat(all_moves[index], positions[x][y], 3);
                    sort[index] = quietMove_score(all_moves[index], hash_move, index_x, index_y, x, y, ply, color);
                    index++;
                }
                else if(pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q') 
                {  
                    strncpy(all_moves[index], positions[index_x][index_y], 3);
                    strncat(all_moves[index], positions[x][y], 3);
                    sort[index] = bCapMove_score(all_moves[index], hash_move, 'k', pos->board[x][y]);
                    index++;
                }
            }
            //rook
            for(int i = 0; i < pos->piece_count[bR]; i++)
            {
                index_x = pos->piece_list[bR][i] / 8;
                index_y = pos->piece_list[bR][i] % 8;
                //up
                for(int j = index_x-1; j >= 0; j--)
                {  
                    if(pos->board[j][index_y] == ' ')
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
                        sort[index] = quietMove_score(all_moves[index], hash_move, index_x, index_y, j, index_y, ply, color);
                        index++;
                    }
                    else if(pos->board[j][index_y] == 'P' || pos->board[j][index_y] == 'R' || pos->board[j][index_y] == 'N' || pos->board[j][index_y] == 'B' || pos->board[j][index_y] == 'Q')
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
                        sort[index] = bCapMove_score(all_moves[index], hash_move, 'r', pos->board[j][index_y]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
                        sort[index] = quietMove_score(all_moves[index], hash_move, index_x, index_y, index_x, j, ply, color);
                        index++;
                    }
                    else if(pos->board[index_x][j] == 'P' || pos->board[index_x][j] == 'R' || pos->board[index_x][j] == 'N' || pos->board[index_x][j] == 'B' || pos->board[index_x][j] == 'Q')
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
                        sort[index] = bCapMove_score(all_moves[index], hash_move, 'r', pos->board[index_x][j]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
                        sort[index] = quietMove_score(all_moves[index], hash_move, index_x, index_y, index_x, j, ply, color);
                        index++;
                    }
                    else if(pos->board[index_x][j] == 'P' || pos->board[index_x][j] == 'R' || pos->board[index_x][j] == 'N' || pos->board[index_x][j] == 'B' || pos->board[index_x][j] == 'Q')
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
                        sort[index] = bCapMove_score(all_moves[index], hash_move, 'r', pos->board[index_x][j]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
                        sort[index] = quietMove_score(all_moves[index], hash_move, index_x, index_y, j, index_y, ply, color);
                        index++;
                    }
                    else if(pos->board[j][index_y] == 'P' || pos->board[j][index_y] == 'R' || pos->board[j][index_y] == 'N' || pos->board[j][index_y] == 'B' || pos->board[j][index_y] == 'Q')
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
                        sort[index] = bCapMove_score(all_moves[index], hash_move, 'r', pos->board[j][index_y]);
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
                index_x = pos->piece_list[bB][i] / 8;
                index_y = pos->piece_list[bB][i] % 8;
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = quietMove_score(all_moves[index], hash_move, index_x, index_y, x, y, ply, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q')  
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = bCapMove_score(all_moves[index], hash_move, 'b', pos->board[x][y]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = quietMove_score(all_moves[index], hash_move, index_x, index_y, x, y, ply, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q') 
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = bCapMove_score(all_moves[index], hash_move, 'b', pos->board[x][y]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = quietMove_score(all_moves[index], hash_move, index_x, index_y, x, y, ply, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q') 
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = bCapMove_score(all_moves[index], hash_move, 'b', pos->board[x][y]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = quietMove_score(all_moves[index], hash_move, index_x, index_y, x, y, ply, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q') 
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = bCapMove_score(all_moves[index], hash_move, 'b', pos->board[x][y]);
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
                index_x = pos->piece_list[bQ][i] / 8;
                index_y = pos->piece_list[bQ][i] % 8;
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = quietMove_score(all_moves[index], hash_move, index_x, index_y, x, y, ply, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q') 
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = bCapMove_score(all_moves[index], hash_move, 'q', pos->board[x][y]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
                        sort[index] = quietMove_score(all_moves[index], hash_move, index_x, index_y, j, index_y, ply, color);
                        index++;
                    }
                    else if(pos->board[j][index_y] == 'P' || pos->board[j][index_y] == 'R' || pos->board[j][index_y] == 'N' || pos->board[j][index_y] == 'B' || pos->board[j][index_y] == 'Q')
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
                        sort[index] = bCapMove_score(all_moves[index], hash_move, 'q', pos->board[j][index_y]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = quietMove_score(all_moves[index], hash_move, index_x, index_y, x, y, ply, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q') 
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = bCapMove_score(all_moves[index], hash_move, 'q', pos->board[x][y]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
                        sort[index] = quietMove_score(all_moves[index], hash_move, index_x, index_y, index_x, j, ply, color);
                        index++;
                    }
                    else if(pos->board[index_x][j] == 'P' || pos->board[index_x][j] == 'R' || pos->board[index_x][j] == 'N' || pos->board[index_x][j] == 'B' || pos->board[index_x][j] == 'Q')
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
                        sort[index] = bCapMove_score(all_moves[index], hash_move, 'q', pos->board[index_x][j]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
                        sort[index] = quietMove_score(all_moves[index], hash_move, index_x, index_y, index_x, j, ply, color);
                        index++;
                    }
                    else if(pos->board[index_x][j] == 'P' || pos->board[index_x][j] == 'R' || pos->board[index_x][j] == 'N' || pos->board[index_x][j] == 'B' || pos->board[index_x][j] == 'Q')
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
                        sort[index] = bCapMove_score(all_moves[index], hash_move, 'q', pos->board[index_x][j]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = quietMove_score(all_moves[index], hash_move, index_x, index_y, x, y, ply, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q') 
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = bCapMove_score(all_moves[index], hash_move, 'q', pos->board[x][y]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
                        sort[index] = quietMove_score(all_moves[index], hash_move, index_x, index_y, j, index_y, ply, color);
                        index++;
                    }
                    else if(pos->board[j][index_y] == 'P' || pos->board[j][index_y] == 'R' || pos->board[j][index_y] == 'N' || pos->board[j][index_y] == 'B' || pos->board[j][index_y] == 'Q')
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
                        sort[index] = bCapMove_score(all_moves[index], hash_move, 'q', pos->board[j][index_y]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = quietMove_score(all_moves[index], hash_move, index_x, index_y, x, y, ply, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q') 
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = bCapMove_score(all_moves[index], hash_move, 'q', pos->board[x][y]);
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
                index_x = pos->piece_list[wP][i] / 8;
                index_y = pos->piece_list[wP][i] % 8;
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
                            strncpy(all_moves[index], positions[index_x][index_y], 3);
                            strncat(all_moves[index], positions[index_x - 1][y], 3);
                            all_moves[index][4] = 'q';
                            all_moves[index][5] = '\0';
                            sort[index] = wCapMove_score(all_moves[index], hash_move, 'P', pos->board[index_x - 1][y]);
                            index++;
                            strncpy(all_moves[index], all_moves[index - 1], 6);
                            all_moves[index][4] = 'n';
                            sort[index] = wCapMove_score(all_moves[index], hash_move, 'P', pos->board[index_x - 1][y]);
                            index++;
                        }
                        else
                        {
                            strncpy(all_moves[index], positions[index_x][index_y], 3);
                            strncat(all_moves[index], positions[index_x - 1][y], 3);
                            sort[index] = wCapMove_score(all_moves[index], hash_move, 'P', pos->board[index_x - 1][y]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x - 1][index_y], 3);
                        all_moves[index][4] = 'q';
                        all_moves[index][5] = '\0';
                        sort[index] = promotion_score(all_moves[index], hash_move);
                        index++;
                        strncpy(all_moves[index], all_moves[index - 1], 6);
                        all_moves[index][4] = 'n';
                        sort[index] = underpromotion_score(all_moves[index], hash_move);
                        index++;
                    }
                    else
                    {
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x - 1][index_y], 3);
                        sort[index] = quietMove_score(all_moves[index], hash_move, index_x, index_y, index_x - 1, index_y, ply, color);
                        index++;
                    }
                }
                //2 steps at starting position
                if(index_x == 6)
                {
                    if(pos->board[5][index_y] == ' ' && pos->board[4][index_y] == ' ')  
                    { 
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[4][index_y], 3);
                        sort[index] = quietMove_score(all_moves[index], hash_move, index_x, index_y, 4, index_y, ply, color);
                        index++;
                    }
                }
            }
            //knight
            for(int i = 0; i < pos->piece_count[wN]; i++)
            {
                index_x = pos->piece_list[wN][i] / 8;
                index_y = pos->piece_list[wN][i] % 8;
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = quietMove_score(all_moves[index], hash_move, index_x, index_y, x, y, ply, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')   
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = wCapMove_score(all_moves[index], hash_move, 'N', pos->board[x][y]);
                        index++;
                    }
                }
            }
            //king
            index_x = pos->piece_list[wK][0] / 8;
            index_y = pos->piece_list[wK][0] % 8;
            //castling
            if(index_x == 7 && index_y == 4)
            {
                if(CheckMove_wkingside(pos))  
                {  
                    strncpy(all_moves[index], positions[index_x][index_y], 3);
                    strncat(all_moves[index], positions[7][6], 3);
                    sort[index] = castling_score(all_moves[index], hash_move);
                    index++;
                }
                if(CheckMove_wqueenside(pos))  
                {  
                    strncpy(all_moves[index], positions[index_x][index_y], 3);
                    strncat(all_moves[index], positions[7][2], 3);
                    sort[index] = castling_score(all_moves[index], hash_move);
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
                    strncpy(all_moves[index], positions[index_x][index_y], 3);
                    strncat(all_moves[index], positions[x][y], 3);
                    sort[index] = quietMove_score(all_moves[index], hash_move, index_x, index_y, x, y, ply, color);
                    index++;
                }
                else if(pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')   
                {  
                    strncpy(all_moves[index], positions[index_x][index_y], 3);
                    strncat(all_moves[index], positions[x][y], 3);
                    sort[index] = wCapMove_score(all_moves[index], hash_move, 'K', pos->board[x][y]);
                    index++;
                }
            }
            //rook
            for(int i = 0; i < pos->piece_count[wR]; i++)
            {
                index_x = pos->piece_list[wR][i] / 8;
                index_y = pos->piece_list[wR][i] % 8;
                //up
                for(int j = index_x-1; j >= 0; j--)
                {
                    if(pos->board[j][index_y] == ' ') 
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
                        sort[index] = quietMove_score(all_moves[index], hash_move, index_x, index_y, j, index_y, ply, color);
                        index++;
                    }
                    else if(pos->board[j][index_y] == 'p' || pos->board[j][index_y] == 'r' || pos->board[j][index_y] == 'n' || pos->board[j][index_y] == 'b' || pos->board[j][index_y] == 'q') 
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
                        sort[index] = wCapMove_score(all_moves[index], hash_move, 'R', pos->board[j][index_y]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
                        sort[index] = quietMove_score(all_moves[index], hash_move, index_x, index_y, index_x, j, ply, color);
                        index++;
                    }
                    else if(pos->board[index_x][j] == 'p' || pos->board[index_x][j] == 'r' || pos->board[index_x][j] == 'n' || pos->board[index_x][j] == 'b' || pos->board[index_x][j] == 'q') 
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
                        sort[index] = wCapMove_score(all_moves[index], hash_move, 'R', pos->board[index_x][j]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
                        sort[index] = quietMove_score(all_moves[index], hash_move, index_x, index_y, index_x, j, ply, color);
                        index++;
                    }
                    else if(pos->board[index_x][j] == 'p' || pos->board[index_x][j] == 'r' || pos->board[index_x][j] == 'n' || pos->board[index_x][j] == 'b' || pos->board[index_x][j] == 'q') 
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
                        sort[index] = wCapMove_score(all_moves[index], hash_move, 'R', pos->board[index_x][j]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
                        sort[index] = quietMove_score(all_moves[index], hash_move, index_x, index_y, j, index_y, ply, color);
                        index++;
                    }
                    else if(pos->board[j][index_y] == 'p' || pos->board[j][index_y] == 'r' || pos->board[j][index_y] == 'n' || pos->board[j][index_y] == 'b' || pos->board[j][index_y] == 'q') 
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
                        sort[index] = wCapMove_score(all_moves[index], hash_move, 'R', pos->board[j][index_y]);
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
                index_x = pos->piece_list[wB][i] / 8;
                index_y = pos->piece_list[wB][i] % 8;
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = quietMove_score(all_moves[index], hash_move, index_x, index_y, x, y, ply, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')  
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = wCapMove_score(all_moves[index], hash_move, 'B', pos->board[x][y]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = quietMove_score(all_moves[index], hash_move, index_x, index_y, x, y, ply, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')  
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = wCapMove_score(all_moves[index], hash_move, 'B', pos->board[x][y]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = quietMove_score(all_moves[index], hash_move, index_x, index_y, x, y, ply, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')  
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = wCapMove_score(all_moves[index], hash_move, 'B', pos->board[x][y]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = quietMove_score(all_moves[index], hash_move, index_x, index_y, x, y, ply, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')  
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = wCapMove_score(all_moves[index], hash_move, 'B', pos->board[x][y]);
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
                    if(pos->board[x][y] == ' ')  
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = quietMove_score(all_moves[index], hash_move, index_x, index_y, x, y, ply, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')  
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = wCapMove_score(all_moves[index], hash_move, 'Q', pos->board[x][y]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
                        sort[index] = quietMove_score(all_moves[index], hash_move, index_x, index_y, j, index_y, ply, color);
                        index++;
                    }
                    else if(pos->board[j][index_y] == 'p' || pos->board[j][index_y] == 'r' || pos->board[j][index_y] == 'n' || pos->board[j][index_y] == 'b' || pos->board[j][index_y] == 'q') 
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
                        sort[index] = wCapMove_score(all_moves[index], hash_move, 'Q', pos->board[j][index_y]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = quietMove_score(all_moves[index], hash_move, index_x, index_y, x, y, ply, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')  
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = wCapMove_score(all_moves[index], hash_move, 'Q', pos->board[x][y]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
                        sort[index] = quietMove_score(all_moves[index], hash_move, index_x, index_y, index_x, j, ply, color);
                        index++;
                    }
                    else if(pos->board[index_x][j] == 'p' || pos->board[index_x][j] == 'r' || pos->board[index_x][j] == 'n' || pos->board[index_x][j] == 'b' || pos->board[index_x][j] == 'q') 
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
                        sort[index] = wCapMove_score(all_moves[index], hash_move, 'Q', pos->board[index_x][j]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
                        sort[index] = quietMove_score(all_moves[index], hash_move, index_x, index_y, index_x, j, ply, color);
                        index++;
                    }
                    else if(pos->board[index_x][j] == 'p' || pos->board[index_x][j] == 'r' || pos->board[index_x][j] == 'n' || pos->board[index_x][j] == 'b' || pos->board[index_x][j] == 'q') 
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
                        sort[index] = wCapMove_score(all_moves[index], hash_move, 'Q', pos->board[index_x][j]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = quietMove_score(all_moves[index], hash_move, index_x, index_y, x, y, ply, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')  
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = wCapMove_score(all_moves[index], hash_move, 'Q', pos->board[x][y]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
                        sort[index] = quietMove_score(all_moves[index], hash_move, index_x, index_y, j, index_y, ply, color);
                        index++;
                    }
                    else if(pos->board[j][index_y] == 'p' || pos->board[j][index_y] == 'r' || pos->board[j][index_y] == 'n' || pos->board[j][index_y] == 'b' || pos->board[j][index_y] == 'q') 
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
                        sort[index] = wCapMove_score(all_moves[index], hash_move, 'Q', pos->board[j][index_y]);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = quietMove_score(all_moves[index], hash_move, index_x, index_y, x, y, ply, color);
                        index++;
                    }
                    else if(pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')  
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = wCapMove_score(all_moves[index], hash_move, 'Q', pos->board[x][y]);
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

//generate caputures and queen promotion
int captureGen(BOARD *pos, char all_moves[100][6], int sort[100], int color)
{
    int index = 0; 
    int index_x;
    int index_y;
    int x, y;

    switch(color)
    {
        //black pieces
        case 1:
        {
            //pawn
            for(int i = 0; i < pos->piece_count[bP]; i++)
            {
                index_x = pos->piece_list[bP][i] / 8;
                index_y = pos->piece_list[bP][i] % 8;
                if((index_y - 1) >= 0 && CheckCapture_bpawn(pos, index_x + 1, index_y - 1)) 
                { 
                    strncpy(all_moves[index], positions[index_x][index_y], 3);
                    strncat(all_moves[index], positions[index_x + 1][index_y - 1], 3);
                    sort[index] = bCapQsearch_score('p', pos->board[index_x + 1][index_y - 1], index_x + 1);
                    index++;
                }
                //promotion
                if(index_x == 6 && pos->board[7][index_y] == ' ')
                {
                    strncpy(all_moves[index], positions[index_x][index_y], 3);
                    strncat(all_moves[index], positions[7][index_y], 3);
                    sort[index] = 1000;
                    index++;
                } 
                if((index_y + 1) <= 7 && CheckCapture_bpawn(pos, index_x + 1, index_y + 1)) 
                { 
                    strncpy(all_moves[index], positions[index_x][index_y], 3);
                    strncat(all_moves[index], positions[index_x + 1][index_y + 1], 3);
                    sort[index] = bCapQsearch_score('p', pos->board[index_x + 1][index_y + 1], index_x + 1);
                    index++;
                }
            }
            //knight
            for(int i = 0; i < pos->piece_count[bN]; i++)
            {
                index_x = pos->piece_list[bN][i] / 8;
                index_y = pos->piece_list[bN][i] % 8;
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = bCapQsearch_score('n', pos->board[x][y], 1);
                        index++;
                    }
                }
            }
            //king
            index_x = pos->piece_list[bK][0] / 8;
            index_y = pos->piece_list[bK][0] % 8;
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
                    strncpy(all_moves[index], positions[index_x][index_y], 3);
                    strncat(all_moves[index], positions[x][y], 3);
                    sort[index] = bCapQsearch_score('k', pos->board[x][y], 1);
                    index++;
                }
            }
            //rook
            for(int i = 0; i < pos->piece_count[bR]; i++)
            {
                index_x = pos->piece_list[bR][i] / 8;
                index_y = pos->piece_list[bR][i] % 8;
                //up
                for(int j = index_x-1; j >= 0; j--)
                {  
                    if(pos->board[j][index_y] == 'P' || pos->board[j][index_y] == 'R' || pos->board[j][index_y] == 'N' || pos->board[j][index_y] == 'B' || pos->board[j][index_y] == 'Q')
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
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
                index_x = pos->piece_list[bB][i] / 8;
                index_y = pos->piece_list[bB][i] % 8;
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
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
                index_x = pos->piece_list[bQ][i] / 8;
                index_y = pos->piece_list[bQ][i] % 8;
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
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
                index_x = pos->piece_list[wP][i] / 8;
                index_y = pos->piece_list[wP][i] % 8;
                if((index_y - 1) >= 0 && CheckCapture_wpawn(pos, index_x - 1, index_y - 1)) 
                { 
                    strncpy(all_moves[index], positions[index_x][index_y], 3);
                    strncat(all_moves[index], positions[index_x - 1][index_y - 1], 3);
                    sort[index] = wCapQsearch_score('P', pos->board[index_x - 1][index_y - 1], index_x - 1);
                    index++;
                }
                //promotion
                if(index_x == 1 && pos->board[0][index_y] == ' ')
                {
                    strncpy(all_moves[index], positions[index_x][index_y], 3);
                    strncat(all_moves[index], positions[0][index_y], 3);
                    sort[index] = 1000;
                    index++;
                } 
                if((index_y + 1) <= 7 && CheckCapture_wpawn(pos, index_x - 1, index_y + 1)) 
                { 
                    strncpy(all_moves[index], positions[index_x][index_y], 3);
                    strncat(all_moves[index], positions[index_x - 1][index_y + 1], 3);
                    sort[index] = wCapQsearch_score('P', pos->board[index_x - 1][index_y + 1], index_x - 1);
                    index++;
                }
            }
            //knight
            for(int i = 0; i < pos->piece_count[wN]; i++)
            {
                index_x = pos->piece_list[wN][i] / 8;
                index_y = pos->piece_list[wN][i] % 8;
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        sort[index] = wCapQsearch_score('N', pos->board[x][y], 1);
                        index++;
                    }
                }
            }
            //king
            index_x = pos->piece_list[wK][0] / 8;
            index_y = pos->piece_list[wK][0] % 8;
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
                    strncpy(all_moves[index], positions[index_x][index_y], 3);
                    strncat(all_moves[index], positions[x][y], 3);
                    sort[index] = wCapQsearch_score('K', pos->board[x][y], 1);
                    index++;
                }
            }
            //rook
            for(int i = 0; i < pos->piece_count[wR]; i++)
            {
                index_x = pos->piece_list[wR][i] / 8;
                index_y = pos->piece_list[wR][i] % 8;
                //up
                for(int j = index_x-1; j >= 0; j--)
                {
                    if(pos->board[j][index_y] == 'p' || pos->board[j][index_y] == 'r' || pos->board[j][index_y] == 'n' || pos->board[j][index_y] == 'b' || pos->board[j][index_y] == 'q') 
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
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
                index_x = pos->piece_list[wB][i] / 8;
                index_y = pos->piece_list[wB][i] % 8;
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
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
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
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

//static exchange evaluation for quiescence search
int SEE(char board[8][8], char location[3], int target, int color)
{
    int attacker_index = get_smallest_attacker(board, location, color);
    //exit if no attackers found
    if(attacker_index == -1)
        return target;
    char board_copy[8][8];
    int x, y;
    char piece;
    bool king_attack = false;
    int gain[32];
    int d = 0;
    gain[d] = target;
    memcpy(board_copy, board, sizeof(board_copy));

    do
    {
        d++;
        //convert index to 2D
        x = attacker_index / 8;
        y = attacker_index % 8;
        
        if(toupper(board_copy[x][y]) == 'K')
            king_attack = true;            

        piece = position_to_piece(board_copy, location);

        makeMove_SEE(positions[x][y], location, board_copy);
        color = -color;
        attacker_index = get_smallest_attacker(board_copy, location, color);
        
        //stop when king is captured
        if(king_attack && attacker_index != -1)
        {
            gain[d] = -gain[d-1];
            break;
        }

        gain[d] = piece_value(piece) - gain[d-1];
        //prunning that won't change the result
        if(((-gain[d-1] >= gain[d])? -gain[d-1]:gain[d]) < 0)
        {
            gain[d] = -gain[d-1];
            break;
        }
    }
    while(attacker_index != -1);

    while(--d)
    {
        gain[d] = -((-gain[d] >= gain[d+1])? -gain[d]:gain[d+1]);
    }
    gain[0] = -((-gain[0] >= gain[1])? -gain[0]:gain[1]);

    return gain[0];
}