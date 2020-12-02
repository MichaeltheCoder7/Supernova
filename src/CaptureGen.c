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

//generate captures and queen promotion
//generate scores for each move
int captureGen(BOARD *pos, MOVE all_moves[256], int sort[256], int color)
{
    int index = 0;
    int index_x;
    int index_y;
    int x, y;
    int origin;

    switch (color)
    {
        //black pieces
        case 1:
        {
            //pawn
            for (int i = 0; i < pos->piece_count[bP]; i++)
            {
                origin = pos->piece_list[bP][i];
                index_x = origin / 8;
                index_y = origin % 8;
                if ((index_y - 1) >= 0 && CheckCapture_bpawn(pos, index_x + 1, index_y - 1))
                {
                    all_moves[index].from = origin;
                    all_moves[index].to = 8 * (index_x + 1) + index_y - 1;
                    sort[index] = bCapQsearch_score('p', pos->board[index_x + 1][index_y - 1]);
                    //capture + promotion
                    if (index_x == 6)
                        sort[index] += 500;
                    index++;
                }
                //promotion
                if (index_x == 6 && pos->board[7][index_y] == ' ')
                {
                    all_moves[index].from = origin;
                    all_moves[index].to = 56 + index_y;
                    sort[index] = 1000;
                    index++;
                }
                if ((index_y + 1) <= 7 && CheckCapture_bpawn(pos, index_x + 1, index_y + 1))
                {
                    all_moves[index].from = origin;
                    all_moves[index].to = 8 * (index_x + 1) + index_y + 1;
                    sort[index] = bCapQsearch_score('p', pos->board[index_x + 1][index_y + 1]);
                    //capture + promotion
                    if (index_x == 6)
                        sort[index] += 500;
                    index++;
                }
            }
            //knight
            for (int i = 0; i < pos->piece_count[bN]; i++)
            {
                origin = pos->piece_list[bN][i];
                index_x = origin / 8;
                index_y = origin % 8;
                for (int j = 0; j < 8; j++)
                {
                    x = index_x + knight_moves_x[j];
                    y = index_y + knight_moves_y[j];
                    if (x & 8 || y & 8) //skip when out of board
                    {
                        continue;
                    }
                    if (isWhitePiece(pos->board[x][y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        sort[index] = bCapQsearch_score('n', pos->board[x][y]);
                        index++;
                    }
                }
            }
            //bishop
            for (int i = 0; i < pos->piece_count[bB]; i++)
            {
                origin = pos->piece_list[bB][i];
                index_x = origin / 8;
                index_y = origin % 8;
                //up left
                for (int j = 1; j <= 7; j++)
                {
                    x = index_x - j;
                    y = index_y - j;
                    if (x < 0 || y < 0)
                    {
                        break;
                    }
                    if (isWhitePiece(pos->board[x][y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        sort[index] = bCapQsearch_score('b', pos->board[x][y]);
                        index++;
                    }
                    if (pos->board[x][y] != ' ')
                    {
                        break;
                    }
                }
                //up right
                for (int j = 1; j <= 7; j++)
                {
                    x = index_x - j;
                    y = index_y + j;
                    if (x < 0 || y > 7)
                    {
                        break;
                    }
                    if (isWhitePiece(pos->board[x][y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        sort[index] = bCapQsearch_score('b', pos->board[x][y]);
                        index++;
                    }
                    if (pos->board[x][y] != ' ')
                    {
                        break;
                    }
                }
                //down left
                for (int j = 1; j <= 7; j++)
                {
                    x = index_x + j;
                    y = index_y - j;
                    if (x > 7 || y < 0)
                    {
                        break;
                    }
                    if (isWhitePiece(pos->board[x][y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        sort[index] = bCapQsearch_score('b', pos->board[x][y]);
                        index++;
                    }
                    if (pos->board[x][y] != ' ')
                    {
                        break;
                    }
                }
                //down right
                for (int j = 1; j <= 7; j++)
                {
                    x = index_x + j;
                    y = index_y + j;
                    if (x > 7 || y > 7)
                    {
                        break;
                    }
                    if (isWhitePiece(pos->board[x][y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        sort[index] = bCapQsearch_score('b', pos->board[x][y]);
                        index++;
                    }
                    if (pos->board[x][y] != ' ')
                    {
                        break;
                    }
                }
            }
            //rook
            for (int i = 0; i < pos->piece_count[bR]; i++)
            {
                origin = pos->piece_list[bR][i];
                index_x = origin / 8;
                index_y = origin % 8;
                //up
                for (int j = index_x - 1; j >= 0; j--)
                {
                    if (isWhitePiece(pos->board[j][index_y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * j + index_y;
                        sort[index] = bCapQsearch_score('r', pos->board[j][index_y]);
                        index++;
                    }
                    if (pos->board[j][index_y] != ' ')
                    {
                        break;
                    }
                }
                //left
                for (int j = index_y - 1; j >= 0; j--)
                {
                    if (isWhitePiece(pos->board[index_x][j]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * index_x + j;
                        sort[index] = bCapQsearch_score('r', pos->board[index_x][j]);
                        index++;
                    }
                    if (pos->board[index_x][j] != ' ')
                    {
                        break;
                    }
                }
                //right
                for (int j = index_y + 1; j <= 7; j++)
                {
                    if (isWhitePiece(pos->board[index_x][j]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * index_x + j;
                        sort[index] = bCapQsearch_score('r', pos->board[index_x][j]);
                        index++;
                    }
                    if (pos->board[index_x][j] != ' ')
                    {
                        break;
                    }
                }
                //down
                for (int j = index_x + 1; j <= 7; j++)
                {
                    if (isWhitePiece(pos->board[j][index_y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * j + index_y;
                        sort[index] = bCapQsearch_score('r', pos->board[j][index_y]);
                        index++;
                    }
                    if (pos->board[j][index_y] != ' ')
                    {
                        break;
                    }
                }
            }
            //queen
            for (int i = 0; i < pos->piece_count[bQ]; i++)
            {
                origin = pos->piece_list[bQ][i];
                index_x = origin / 8;
                index_y = origin % 8;
                //up left
                for (int j = 1; j <= 7; j++)
                {
                    x = index_x - j;
                    y = index_y - j;
                    if (x < 0 || y < 0)
                    {
                        break;
                    }
                    if (isWhitePiece(pos->board[x][y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        sort[index] = bCapQsearch_score('q', pos->board[x][y]);
                        index++;
                    }
                    if (pos->board[x][y] != ' ')
                    {
                        break;
                    }
                }
                //up
                for (int j = index_x - 1; j >= 0; j--)
                {
                    if (isWhitePiece(pos->board[j][index_y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * j + index_y;
                        sort[index] = bCapQsearch_score('q', pos->board[j][index_y]);
                        index++;
                    }
                    if (pos->board[j][index_y] != ' ')
                    {
                        break;
                    }
                }
                //up right
                for (int j = 1; j <= 7; j++)
                {
                    x = index_x - j;
                    y = index_y + j;
                    if (x < 0 || y > 7)
                    {
                        break;
                    }
                    if (isWhitePiece(pos->board[x][y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        sort[index] = bCapQsearch_score('q', pos->board[x][y]);
                        index++;
                    }
                    if (pos->board[x][y] != ' ')
                    {
                        break;
                    }
                }
                //left
                for (int j = index_y - 1; j >= 0; j--)
                {
                    if (isWhitePiece(pos->board[index_x][j]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * index_x + j;
                        sort[index] = bCapQsearch_score('q', pos->board[index_x][j]);
                        index++;
                    }
                    if (pos->board[index_x][j] != ' ')
                    {
                        break;
                    }
                }
                //right
                for (int j = index_y + 1; j <= 7; j++)
                {
                    if (isWhitePiece(pos->board[index_x][j]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * index_x + j;
                        sort[index] = bCapQsearch_score('q', pos->board[index_x][j]);
                        index++;
                    }
                    if (pos->board[index_x][j] != ' ')
                    {
                        break;
                    }
                }
                //down left
                for (int j = 1; j <= 7; j++)
                {
                    x = index_x + j;
                    y = index_y - j;
                    if (x > 7 || y < 0)
                    {
                        break;
                    }
                    if (isWhitePiece(pos->board[x][y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        sort[index] = bCapQsearch_score('q', pos->board[x][y]);
                        index++;
                    }
                    if (pos->board[x][y] != ' ')
                    {
                        break;
                    }
                }
                //down
                for (int j = index_x + 1; j <= 7; j++)
                {
                    if (isWhitePiece(pos->board[j][index_y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * j + index_y;
                        sort[index] = bCapQsearch_score('q', pos->board[j][index_y]);
                        index++;
                    }
                    if (pos->board[j][index_y] != ' ')
                    {
                        break;
                    }
                }
                //down right
                for (int j = 1; j <= 7; j++)
                {
                    x = index_x + j;
                    y = index_y + j;
                    if (x > 7 || y > 7)
                    {
                        break;
                    }
                    if (isWhitePiece(pos->board[x][y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        sort[index] = bCapQsearch_score('q', pos->board[x][y]);
                        index++;
                    }
                    if (pos->board[x][y] != ' ')
                    {
                        break;
                    }
                }
            }
            //king
            origin = pos->piece_list[bK][0];
            index_x = origin / 8;
            index_y = origin % 8;
            for (int j = 0; j < 8; j++)
            {
                x = index_x + king_moves_x[j];
                y = index_y + king_moves_y[j];
                if (x & 8 || y & 8) //skip when out of board
                {
                    continue;
                }
                if (isWhitePiece(pos->board[x][y]))
                {
                    all_moves[index].from = origin;
                    all_moves[index].to = 8 * x + y;
                    sort[index] = bCapQsearch_score('k', pos->board[x][y]);
                    index++;
                }
            }
            break;
        }
        //white pieces
        case -1:
        {
            //pawn
            for (int i = 0; i < pos->piece_count[wP]; i++)
            {
                origin = pos->piece_list[wP][i];
                index_x = origin / 8;
                index_y = origin % 8;
                if ((index_y - 1) >= 0 && CheckCapture_wpawn(pos, index_x - 1, index_y - 1))
                {
                    all_moves[index].from = origin;
                    all_moves[index].to = 8 * (index_x - 1) + index_y - 1;
                    sort[index] = wCapQsearch_score('P', pos->board[index_x - 1][index_y - 1]);
                    //capture + promotion
                    if (index_x == 1)
                        sort[index] += 500;
                    index++;
                }
                //promotion
                if (index_x == 1 && pos->board[0][index_y] == ' ')
                {
                    all_moves[index].from = origin;
                    all_moves[index].to = index_y;
                    sort[index] = 1000;
                    index++;
                }
                if ((index_y + 1) <= 7 && CheckCapture_wpawn(pos, index_x - 1, index_y + 1))
                {
                    all_moves[index].from = origin;
                    all_moves[index].to = 8 * (index_x - 1) + index_y + 1;
                    sort[index] = wCapQsearch_score('P', pos->board[index_x - 1][index_y + 1]);
                    //capture + promotion
                    if (index_x == 1)
                        sort[index] += 500;
                    index++;
                }
            }
            //knight
            for (int i = 0; i < pos->piece_count[wN]; i++)
            {
                origin = pos->piece_list[wN][i];
                index_x = origin / 8;
                index_y = origin % 8;
                for (int j = 0; j < 8; j++)
                {
                    x = index_x + knight_moves_x[j];
                    y = index_y + knight_moves_y[j];
                    if (x & 8 || y & 8) //skip when out of board
                    {
                        continue;
                    }
                    if (isBlackPiece(pos->board[x][y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        sort[index] = wCapQsearch_score('N', pos->board[x][y]);
                        index++;
                    }
                }
            }
            //bishop
            for (int i = 0; i < pos->piece_count[wB]; i++)
            {
                origin = pos->piece_list[wB][i];
                index_x = origin / 8;
                index_y = origin % 8;
                //up left
                for (int j = 1; j <= 7; j++)
                {
                    x = index_x - j;
                    y = index_y - j;
                    if (x < 0 || y < 0)
                    {
                        break;
                    }
                    if (isBlackPiece(pos->board[x][y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        sort[index] = wCapQsearch_score('B', pos->board[x][y]);
                        index++;
                    }
                    if (pos->board[x][y] != ' ')
                    {
                        break;
                    }
                }
                //up right
                for (int j = 1; j <= 7; j++)
                {
                    x = index_x - j;
                    y = index_y + j;
                    if (x < 0 || y > 7)
                    {
                        break;
                    }
                    if (isBlackPiece(pos->board[x][y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        sort[index] = wCapQsearch_score('B', pos->board[x][y]);
                        index++;
                    }
                    if (pos->board[x][y] != ' ')
                    {
                        break;
                    }
                }
                //down left
                for (int j = 1; j <= 7; j++)
                {
                    x = index_x + j;
                    y = index_y - j;
                    if (x > 7 || y < 0)
                    {
                        break;
                    }
                    if (isBlackPiece(pos->board[x][y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        sort[index] = wCapQsearch_score('B', pos->board[x][y]);
                        index++;
                    }
                    if (pos->board[x][y] != ' ')
                    {
                        break;
                    }
                }
                //down right
                for (int j = 1; j <= 7; j++)
                {
                    x = index_x + j;
                    y = index_y + j;
                    if (x > 7 || y > 7)
                    {
                        break;
                    }
                    if (isBlackPiece(pos->board[x][y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        sort[index] = wCapQsearch_score('B', pos->board[x][y]);
                        index++;
                    }
                    if (pos->board[x][y] != ' ')
                    {
                        break;
                    }
                }
            }
            //rook
            for (int i = 0; i < pos->piece_count[wR]; i++)
            {
                origin = pos->piece_list[wR][i];
                index_x = origin / 8;
                index_y = origin % 8;
                //up
                for (int j = index_x - 1; j >= 0; j--)
                {
                    if (isBlackPiece(pos->board[j][index_y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * j + index_y;
                        sort[index] = wCapQsearch_score('R', pos->board[j][index_y]);
                        index++;
                    }
                    if (pos->board[j][index_y] != ' ')
                    {
                        break;
                    }
                }
                //left
                for (int j = index_y - 1; j >= 0; j--)
                {
                    if (isBlackPiece(pos->board[index_x][j]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * index_x + j;
                        sort[index] = wCapQsearch_score('R', pos->board[index_x][j]);
                        index++;
                    }
                    if (pos->board[index_x][j] != ' ')
                    {
                        break;
                    }
                }
                //right
                for (int j = index_y + 1; j <= 7; j++)
                {
                    if (isBlackPiece(pos->board[index_x][j]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * index_x + j;
                        sort[index] = wCapQsearch_score('R', pos->board[index_x][j]);
                        index++;
                    }
                    if (pos->board[index_x][j] != ' ')
                    {
                        break;
                    }
                }
                //down
                for (int j = index_x + 1; j <= 7; j++)
                {
                    if (isBlackPiece(pos->board[j][index_y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * j + index_y;
                        sort[index] = wCapQsearch_score('R', pos->board[j][index_y]);
                        index++;
                    }
                    if (pos->board[j][index_y] != ' ')
                    {
                        break;
                    }
                }
            }
            //queen
            for (int i = 0; i < pos->piece_count[wQ]; i++)
            {
                origin = pos->piece_list[wQ][i];
                index_x = pos->piece_list[wQ][i] / 8;
                index_y = pos->piece_list[wQ][i] % 8;
                //up left
                for (int j = 1; j <= 7; j++)
                {
                    x = index_x - j;
                    y = index_y - j;
                    if (x < 0 || y < 0)
                    {
                        break;
                    }
                    if (isBlackPiece(pos->board[x][y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        sort[index] = wCapQsearch_score('Q', pos->board[x][y]);
                        index++;
                    }
                    if (pos->board[x][y] != ' ')
                    {
                        break;
                    }
                }
                //up
                for (int j = index_x - 1; j >= 0; j--)
                {
                    if (isBlackPiece(pos->board[j][index_y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * j + index_y;
                        sort[index] = wCapQsearch_score('Q', pos->board[j][index_y]);
                        index++;
                    }
                    if (pos->board[j][index_y] != ' ')
                    {
                        break;
                    }
                }
                //up right
                for (int j = 1; j <= 7; j++)
                {
                    x = index_x - j;
                    y = index_y + j;
                    if (x < 0 || y > 7)
                    {
                        break;
                    }
                    if (isBlackPiece(pos->board[x][y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        sort[index] = wCapQsearch_score('Q', pos->board[x][y]);
                        index++;
                    }
                    if (pos->board[x][y] != ' ')
                    {
                        break;
                    }
                }
                //left
                for (int j = index_y - 1; j >= 0; j--)
                {
                    if (isBlackPiece(pos->board[index_x][j]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * index_x + j;
                        sort[index] = wCapQsearch_score('Q', pos->board[index_x][j]);
                        index++;
                    }
                    if (pos->board[index_x][j] != ' ')
                    {
                        break;
                    }
                }
                //right
                for (int j = index_y + 1; j <= 7; j++)
                {
                    if (isBlackPiece(pos->board[index_x][j]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * index_x + j;
                        sort[index] = wCapQsearch_score('Q', pos->board[index_x][j]);
                        index++;
                    }
                    if (pos->board[index_x][j] != ' ')
                    {
                        break;
                    }
                }
                //down left
                for (int j = 1; j <= 7; j++)
                {
                    x = index_x + j;
                    y = index_y - j;
                    if (x > 7 || y < 0)
                    {
                        break;
                    }
                    if (isBlackPiece(pos->board[x][y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        sort[index] = wCapQsearch_score('Q', pos->board[x][y]);
                        index++;
                    }
                    if (pos->board[x][y] != ' ')
                    {
                        break;
                    }
                }
                //down
                for (int j = index_x + 1; j <= 7; j++)
                {
                    if (isBlackPiece(pos->board[j][index_y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * j + index_y;
                        sort[index] = wCapQsearch_score('Q', pos->board[j][index_y]);
                        index++;
                    }
                    if (pos->board[j][index_y] != ' ')
                    {
                        break;
                    }
                }
                //down right
                for (int j = 1; j <= 7; j++)
                {
                    x = index_x + j;
                    y = index_y + j;
                    if (x > 7 || y > 7)
                    {
                        break;
                    }
                    if (isBlackPiece(pos->board[x][y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        sort[index] = wCapQsearch_score('Q', pos->board[x][y]);
                        index++;
                    }
                    if (pos->board[x][y] != ' ')
                    {
                        break;
                    }
                }
            }
            //king
            origin = pos->piece_list[wK][0];
            index_x = origin / 8;
            index_y = origin % 8;
            for (int j = 0; j < 8; j++)
            {
                x = index_x + king_moves_x[j];
                y = index_y + king_moves_y[j];
                if (x & 8 || y & 8) //skip when out of board
                {
                    continue;
                }
                if (isBlackPiece(pos->board[x][y]))
                {
                    all_moves[index].from = origin;
                    all_moves[index].to = 8 * x + y;
                    sort[index] = wCapQsearch_score('K', pos->board[x][y]);
                    index++;
                }
            }
            break;
        }
    }

    return index;
}