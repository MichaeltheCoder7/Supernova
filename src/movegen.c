#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>
#include "movegen.h"
#include "checkmove.h"
#include "ordermove.h"
#include "board.h"
#include "move.h"

const int knight_moves_x[8] = { -2, -2, -1, -1,  1,  1,  2,  2 };
const int knight_moves_y[8] = { -1,  1, -2,  2, -2,  2, -1,  1 };
const int king_moves_x[8] = { -1, -1, -1,  0,  0,  1,  1,  1 };
const int king_moves_y[8] = { -1,  0,  1, -1,  1, -1,  0,  1 };

// generate all pseudo-legal moves
// assign scores for each move
int moveGen(BOARD *pos, MOVE all_moves[256], int sort[256], int ply, int color)
{
    int index = 0;
    int index_x;
    int index_y;
    int x, y;
    int origin;

    switch (color)
    {
        // black pieces
        case 1:
        {
            // pawn
            for (int i = 0; i < pos->piece_count[bP]; i++)
            {
                origin = pos->piece_list[bP][i];
                index_x = origin / 8;
                index_y = origin % 8;
                // capture
                for (y = index_y - 1; y < index_y + 2; y += 2)
                {
                    if (y & 8) // skip when out of board
                    {
                        continue;
                    }
                    if (checkCapture_bpawn(pos, index_x + 1, y))
                    {
                        // promotions
                        if (index_x == 6)
                        {
                            all_moves[index].from = origin;
                            all_moves[index].to = 8 * (index_x + 1) + y;
                            all_moves[index].promotion = 'q';
                            sort[index] = capMove_score(bP, pos->board[index_x + 1][y], pos->board, index_x, index_y, index_x + 1, y, color) + 500;
                            index++;
                            all_moves[index].from = origin;
                            all_moves[index].to = 8 * (index_x + 1) + y;
                            all_moves[index].promotion = 'r';
                            sort[index] = UNDERPROM;
                            index++;
                            all_moves[index].from = origin;
                            all_moves[index].to = 8 * (index_x + 1) + y;
                            all_moves[index].promotion = 'b';
                            sort[index] = UNDERPROM;
                            index++;
                            all_moves[index].from = origin;
                            all_moves[index].to = 8 * (index_x + 1) + y;
                            all_moves[index].promotion = 'n';
                            sort[index] = UNDERPROM;
                            index++;
                        }
                        else
                        {
                            all_moves[index].from = origin;
                            all_moves[index].to = 8 * (index_x + 1) + y;
                            all_moves[index].promotion = ' ';
                            sort[index] = capMove_score(bP, pos->board[index_x + 1][y], pos->board, index_x, index_y, index_x + 1, y, color);
                            index++;
                        }
                    }
                }
                // 1 step
                if (pos->board[index_x + 1][index_y] == __)
                {
                    // promotions
                    if (index_x == 6)
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * (index_x + 1) + index_y;
                        all_moves[index].promotion = 'q';
                        sort[index] = PROMOTION;
                        index++;
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * (index_x + 1) + index_y;
                        all_moves[index].promotion = 'r';
                        sort[index] = UNDERPROM;
                        index++;
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * (index_x + 1) + index_y;
                        all_moves[index].promotion = 'b';
                        sort[index] = UNDERPROM;
                        index++;
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * (index_x + 1) + index_y;
                        all_moves[index].promotion = 'n';
                        sort[index] = UNDERPROM;
                        index++;
                    }
                    else
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * (index_x + 1) + index_y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(pos, &all_moves[index], origin, index_x + 1, index_y, ply, color);
                        index++;
                    }
                }
                // 2 steps at starting position
                if (index_x == 1)
                {
                    if (pos->board[2][index_y] == __ && pos->board[3][index_y] == __)
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 24 + index_y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(pos, &all_moves[index], origin, 3, index_y, ply, color);
                        index++;
                    }
                }
            }

            // knight
            for (int i = 0; i < pos->piece_count[bN]; i++)
            {
                origin = pos->piece_list[bN][i];
                index_x = origin / 8;
                index_y = origin % 8;
                for (int j = 0; j < 8; j++)
                {
                    x = index_x + knight_moves_x[j];
                    y = index_y + knight_moves_y[j];
                    if (x & 8 || y & 8) // skip when out of board
                    {
                        continue;
                    }
                    if (pos->board[x][y] == __)
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(pos, &all_moves[index], origin, x, y, ply, color);
                        index++;
                    }
                    else if (isWhitePiece(pos->board[x][y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        all_moves[index].promotion = ' ';
                        sort[index] = capMove_score(bN, pos->board[x][y], pos->board, index_x, index_y, x, y, color);
                        index++;
                    }
                }
            }

            // bishop
            for (int i = 0; i < pos->piece_count[bB]; i++)
            {
                origin = pos->piece_list[bB][i];
                index_x = origin / 8;
                index_y = origin % 8;
                // up left
                for (int j = 1; j <= 7; j++)
                {
                    x = index_x - j;
                    y = index_y - j;
                    if (x < 0 || y < 0)
                    {
                        break;
                    }
                    if (pos->board[x][y] == __)
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(pos, &all_moves[index], origin, x, y, ply, color);
                        index++;
                    }
                    else if (isWhitePiece(pos->board[x][y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        all_moves[index].promotion = ' ';
                        sort[index] = capMove_score(bB, pos->board[x][y], pos->board, index_x, index_y, x, y, color);
                        index++;
                    }
                    if (pos->board[x][y] != __)
                    {
                        break;
                    }
                }
                // up right
                for (int j = 1; j <= 7; j++)
                {
                    x = index_x - j;
                    y = index_y + j;
                    if (x < 0 || y > 7)
                    {
                        break;
                    }
                    if (pos->board[x][y] == __)
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(pos, &all_moves[index], origin, x, y, ply, color);
                        index++;
                    }
                    else if (isWhitePiece(pos->board[x][y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        all_moves[index].promotion = ' ';
                        sort[index] = capMove_score(bB, pos->board[x][y], pos->board, index_x, index_y, x, y, color);
                        index++;
                    }
                    if (pos->board[x][y] != __)
                    {
                        break;
                    }
                }
                // down left
                for (int j = 1; j <= 7; j++)
                {
                    x = index_x + j;
                    y = index_y - j;
                    if (x > 7 || y < 0)
                    {
                        break;
                    }
                    if (pos->board[x][y] == __)
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(pos, &all_moves[index], origin, x, y, ply, color);
                        index++;
                    }
                    else if (isWhitePiece(pos->board[x][y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        all_moves[index].promotion = ' ';
                        sort[index] = capMove_score(bB, pos->board[x][y], pos->board, index_x, index_y, x, y, color);
                        index++;
                    }
                    if (pos->board[x][y] != __)
                    {
                        break;
                    }
                }
                // down right
                for (int j = 1; j <= 7; j++)
                {
                    x = index_x + j;
                    y = index_y + j;
                    if (x > 7 || y > 7)
                    {
                        break;
                    }
                    if (pos->board[x][y] == __)
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(pos, &all_moves[index], origin, x, y, ply, color);
                        index++;
                    }
                    else if (isWhitePiece(pos->board[x][y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        all_moves[index].promotion = ' ';
                        sort[index] = capMove_score(bB, pos->board[x][y], pos->board, index_x, index_y, x, y, color);
                        index++;
                    }
                    if (pos->board[x][y] != __)
                    {
                        break;
                    }
                }
            }

            // rook
            for (int i = 0; i < pos->piece_count[bR]; i++)
            {
                origin = pos->piece_list[bR][i];
                index_x = pos->piece_list[bR][i] / 8;
                index_y = pos->piece_list[bR][i] % 8;
                // up
                for (int j = index_x - 1; j >= 0; j--)
                {
                    if (pos->board[j][index_y] == __)
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * j + index_y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(pos, &all_moves[index], origin, j, index_y, ply, color);
                        index++;
                    }
                    else if (isWhitePiece(pos->board[j][index_y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * j + index_y;
                        all_moves[index].promotion = ' ';
                        sort[index] = capMove_score(bR, pos->board[j][index_y], pos->board, index_x, index_y, j, index_y, color);
                        index++;
                    }
                    if (pos->board[j][index_y] != __)
                    {
                        break;
                    }
                }
                // left
                for (int j = index_y - 1; j >= 0; j--)
                {
                    if (pos->board[index_x][j] == __)
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * index_x + j;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(pos, &all_moves[index], origin, index_x, j, ply, color);
                        index++;
                    }
                    else if (isWhitePiece(pos->board[index_x][j]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * index_x + j;
                        all_moves[index].promotion = ' ';
                        sort[index] = capMove_score(bR, pos->board[index_x][j], pos->board, index_x, index_y, index_x, j, color);
                        index++;
                    }
                    if (pos->board[index_x][j] != __)
                    {
                        break;
                    }
                }
                // right
                for (int j = index_y + 1; j <= 7; j++)
                {
                    if (pos->board[index_x][j] == __)
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * index_x + j;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(pos, &all_moves[index], origin, index_x, j, ply, color);
                        index++;
                    }
                    else if (isWhitePiece(pos->board[index_x][j]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * index_x + j;
                        all_moves[index].promotion = ' ';
                        sort[index] = capMove_score(bR, pos->board[index_x][j], pos->board, index_x, index_y, index_x, j, color);
                        index++;
                    }
                    if (pos->board[index_x][j] != __)
                    {
                        break;
                    }
                }
                // down
                for (int j = index_x + 1; j <= 7; j++)
                {
                    if (pos->board[j][index_y] == __)
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * j + index_y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(pos, &all_moves[index], origin, j, index_y, ply, color);
                        index++;
                    }
                    else if (isWhitePiece(pos->board[j][index_y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * j + index_y;
                        all_moves[index].promotion = ' ';
                        sort[index] = capMove_score(bR, pos->board[j][index_y], pos->board, index_x, index_y, j, index_y, color);
                        index++;
                    }
                    if (pos->board[j][index_y] != __)
                    {
                        break;
                    }
                }
            }

            // queen
            for (int i = 0; i < pos->piece_count[bQ]; i++)
            {
                origin = pos->piece_list[bQ][i];
                index_x = origin / 8;
                index_y = origin % 8;
                // up left
                for (int j = 1; j <= 7; j++)
                {
                    x = index_x - j;
                    y = index_y - j;
                    if (x < 0 || y < 0)
                    {
                        break;
                    }
                    if (pos->board[x][y] == __)
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(pos, &all_moves[index], origin, x, y, ply, color);
                        index++;
                    }
                    else if (isWhitePiece(pos->board[x][y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        all_moves[index].promotion = ' ';
                        sort[index] = capMove_score(bQ, pos->board[x][y], pos->board, index_x, index_y, x, y, color);
                        index++;
                    }
                    if (pos->board[x][y] != __)
                    {
                        break;
                    }
                }
                // up
                for (int j = index_x - 1; j >= 0; j--)
                {
                    if (pos->board[j][index_y] == __)
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * j + index_y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(pos, &all_moves[index], origin, j, index_y, ply, color);
                        index++;
                    }
                    else if (isWhitePiece(pos->board[j][index_y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * j + index_y;
                        all_moves[index].promotion = ' ';
                        sort[index] = capMove_score(bQ, pos->board[j][index_y], pos->board, index_x, index_y, j, index_y, color);
                        index++;
                    }
                    if (pos->board[j][index_y] != __)
                    {
                        break;
                    }
                }
                // up right
                for (int j = 1; j <= 7; j++)
                {
                    x = index_x - j;
                    y = index_y + j;
                    if (x < 0 || y > 7)
                    {
                        break;
                    }
                    if (pos->board[x][y] == __)
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(pos, &all_moves[index], origin, x, y, ply, color);
                        index++;
                    }
                    else if (isWhitePiece(pos->board[x][y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        all_moves[index].promotion = ' ';
                        sort[index] = capMove_score(bQ, pos->board[x][y], pos->board, index_x, index_y, x, y, color);
                        index++;
                    }
                    if (pos->board[x][y] != __)
                    {
                        break;
                    }
                }
                // left
                for (int j = index_y - 1; j >= 0; j--)
                {
                    if (pos->board[index_x][j] == __)
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * index_x + j;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(pos, &all_moves[index], origin, index_x, j, ply, color);
                        index++;
                    }
                    else if (isWhitePiece(pos->board[index_x][j]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * index_x + j;
                        all_moves[index].promotion = ' ';
                        sort[index] = capMove_score(bQ, pos->board[index_x][j], pos->board, index_x, index_y, index_x, j, color);
                        index++;
                    }
                    if (pos->board[index_x][j] != __)
                    {
                        break;
                    }
                }
                // right
                for (int j = index_y + 1; j <= 7; j++)
                {
                    if (pos->board[index_x][j] == __)
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * index_x + j;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(pos, &all_moves[index], origin, index_x, j, ply, color);
                        index++;
                    }
                    else if (isWhitePiece(pos->board[index_x][j]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * index_x + j;
                        all_moves[index].promotion = ' ';
                        sort[index] = capMove_score(bQ, pos->board[index_x][j], pos->board, index_x, index_y, index_x, j, color);
                        index++;
                    }
                    if (pos->board[index_x][j] != __)
                    {
                        break;
                    }
                }
                // down left
                for (int j = 1; j <= 7; j++)
                {
                    x = index_x + j;
                    y = index_y - j;
                    if (x > 7 || y < 0)
                    {
                        break;
                    }
                    if (pos->board[x][y] == __)
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(pos, &all_moves[index], origin, x, y, ply, color);
                        index++;
                    }
                    else if (isWhitePiece(pos->board[x][y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        all_moves[index].promotion = ' ';
                        sort[index] = capMove_score(bQ, pos->board[x][y], pos->board, index_x, index_y, x, y, color);
                        index++;
                    }
                    if (pos->board[x][y] != __)
                    {
                        break;
                    }
                }
                // down
                for (int j = index_x + 1; j <= 7; j++)
                {
                    if (pos->board[j][index_y] == __)
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * j + index_y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(pos, &all_moves[index], origin, j, index_y, ply, color);
                        index++;
                    }
                    else if (isWhitePiece(pos->board[j][index_y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * j + index_y;
                        all_moves[index].promotion = ' ';
                        sort[index] = capMove_score(bQ, pos->board[j][index_y], pos->board, index_x, index_y, j, index_y, color);
                        index++;
                    }
                    if (pos->board[j][index_y] != __)
                    {
                        break;
                    }
                }
                // down right
                for (int j = 1; j <= 7; j++)
                {
                    x = index_x + j;
                    y = index_y + j;
                    if (x > 7 || y > 7)
                    {
                        break;
                    }
                    if (pos->board[x][y] == __)
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(pos, &all_moves[index], origin, x, y, ply, color);
                        index++;
                    }
                    else if (isWhitePiece(pos->board[x][y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        all_moves[index].promotion = ' ';
                        sort[index] = capMove_score(bQ, pos->board[x][y], pos->board, index_x, index_y, x, y, color);
                        index++;
                    }
                    if (pos->board[x][y] != __)
                    {
                        break;
                    }
                }
            }

            // king
            origin = pos->piece_list[bK][0];
            index_x = origin / 8;
            index_y = origin % 8;
            // castling
            if (origin == e8)
            {
                if (checkMove_bkingside(pos))
                {
                    all_moves[index].from = origin;
                    all_moves[index].to = g8;
                    all_moves[index].promotion = ' ';
                    sort[index] = CASTLING;
                    index++;
                }
                if (checkMove_bqueenside(pos))
                {
                    all_moves[index].from = origin;
                    all_moves[index].to = c8;
                    all_moves[index].promotion = ' ';
                    sort[index] = CASTLING;
                    index++;
                }
            }
            for (int j = 0; j < 8; j++)
            {
                x = index_x + king_moves_x[j];
                y = index_y + king_moves_y[j];
                if (x & 8 || y & 8) // skip when out of board
                {
                    continue;
                }
                if (pos->board[x][y] == __)
                {
                    all_moves[index].from = origin;
                    all_moves[index].to = 8 * x + y;
                    all_moves[index].promotion = ' ';
                    sort[index] = quietMove_score(pos, &all_moves[index], origin, x, y, ply, color);
                    index++;
                }
                else if (isWhitePiece(pos->board[x][y]))
                {
                    all_moves[index].from = origin;
                    all_moves[index].to = 8 * x + y;
                    all_moves[index].promotion = ' ';
                    sort[index] = capMove_score(bK, pos->board[x][y], pos->board, index_x, index_y, x, y, color);
                    index++;
                }
            }
            break;
        }
        // white pieces
        case -1:
        {
            // pawn
            for (int i = 0; i < pos->piece_count[wP]; i++)
            {
                origin = pos->piece_list[wP][i];
                index_x = origin / 8;
                index_y = origin % 8;
                for (y = index_y - 1; y < index_y + 2; y += 2)
                {
                    if (y & 8) // skip when out of board
                    {
                        continue;
                    }
                    if (checkCapture_wpawn(pos, index_x - 1, y))
                    {
                        // promotions
                        if (index_x == 1)
                        {
                            all_moves[index].from = origin;
                            all_moves[index].to = 8 * (index_x - 1) + y;
                            all_moves[index].promotion = 'q';
                            sort[index] = capMove_score(wP, pos->board[index_x - 1][y], pos->board, index_x, index_y, index_x - 1, y, color) + 500;
                            index++;
                            all_moves[index].from = origin;
                            all_moves[index].to = 8 * (index_x - 1) + y;
                            all_moves[index].promotion = 'r';
                            sort[index] = UNDERPROM;
                            index++;
                            all_moves[index].from = origin;
                            all_moves[index].to = 8 * (index_x - 1) + y;
                            all_moves[index].promotion = 'b';
                            sort[index] = UNDERPROM;
                            index++;
                            all_moves[index].from = origin;
                            all_moves[index].to = 8 * (index_x - 1) + y;
                            all_moves[index].promotion = 'n';
                            sort[index] = UNDERPROM;
                            index++;
                        }
                        else
                        {
                            all_moves[index].from = origin;
                            all_moves[index].to = 8 * (index_x - 1) + y;
                            all_moves[index].promotion = ' ';
                            sort[index] = capMove_score(wP, pos->board[index_x - 1][y], pos->board, index_x, index_y, index_x - 1, y, color);
                            index++;
                        }
                    }
                }
                // 1 step
                if (pos->board[index_x - 1][index_y] == __)
                {
                    // promotions
                    if (index_x == 1)
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * (index_x - 1) + index_y;
                        all_moves[index].promotion = 'q';
                        sort[index] = PROMOTION;
                        index++;
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * (index_x - 1) + index_y;
                        all_moves[index].promotion = 'r';
                        sort[index] = UNDERPROM;
                        index++;
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * (index_x - 1) + index_y;
                        all_moves[index].promotion = 'b';
                        sort[index] = UNDERPROM;
                        index++;
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * (index_x - 1) + index_y;
                        all_moves[index].promotion = 'n';
                        sort[index] = UNDERPROM;
                        index++;
                    }
                    else
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * (index_x - 1) + index_y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(pos, &all_moves[index], origin, index_x - 1, index_y, ply, color);
                        index++;
                    }
                }
                // 2 steps at starting position
                if (index_x == 6)
                {
                    if (pos->board[5][index_y] == __ && pos->board[4][index_y] == __)
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 32 + index_y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(pos, &all_moves[index], origin, 4, index_y, ply, color);
                        index++;
                    }
                }
            }

            // knight
            for (int i = 0; i < pos->piece_count[wN]; i++)
            {
                origin = pos->piece_list[wN][i];
                index_x = origin / 8;
                index_y = origin % 8;
                for (int j = 0; j < 8; j++)
                {
                    x = index_x + knight_moves_x[j];
                    y = index_y + knight_moves_y[j];
                    if (x & 8 || y & 8) // skip when out of board
                    {
                        continue;
                    }
                    if (pos->board[x][y] == __)
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(pos, &all_moves[index], origin, x, y, ply, color);
                        index++;
                    }
                    else if (isBlackPiece(pos->board[x][y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        all_moves[index].promotion = ' ';
                        sort[index] = capMove_score(wN, pos->board[x][y], pos->board, index_x, index_y, x, y, color);
                        index++;
                    }
                }
            }

            // bishop
            for (int i = 0; i < pos->piece_count[wB]; i++)
            {
                origin = pos->piece_list[wB][i];
                index_x = origin / 8;
                index_y = origin % 8;
                // up left
                for (int j = 1; j <= 7; j++)
                {
                    x = index_x - j;
                    y = index_y - j;
                    if (x < 0 || y < 0)
                    {
                        break;
                    }
                    if (pos->board[x][y] == __)
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(pos, &all_moves[index], origin, x, y, ply, color);
                        index++;
                    }
                    else if (isBlackPiece(pos->board[x][y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        all_moves[index].promotion = ' ';
                        sort[index] = capMove_score(wB, pos->board[x][y], pos->board, index_x, index_y, x, y, color);
                        index++;
                    }
                    if (pos->board[x][y] != __)
                    {
                        break;
                    }
                }
                // up right
                for (int j = 1; j <= 7; j++)
                {
                    x = index_x - j;
                    y = index_y + j;
                    if (x < 0 || y > 7)
                    {
                        break;
                    }
                    if (pos->board[x][y] == __)
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(pos, &all_moves[index], origin, x, y, ply, color);
                        index++;
                    }
                    else if (isBlackPiece(pos->board[x][y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        all_moves[index].promotion = ' ';
                        sort[index] = capMove_score(wB, pos->board[x][y], pos->board, index_x, index_y, x, y, color);
                        index++;
                    }
                    if (pos->board[x][y] != __)
                    {
                        break;
                    }
                }
                // down left
                for (int j = 1; j <= 7; j++)
                {
                    x = index_x + j;
                    y = index_y - j;
                    if (x > 7 || y < 0)
                    {
                        break;
                    }
                    if (pos->board[x][y] == __)
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(pos, &all_moves[index], origin, x, y, ply, color);
                        index++;
                    }
                    else if (isBlackPiece(pos->board[x][y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        all_moves[index].promotion = ' ';
                        sort[index] = capMove_score(wB, pos->board[x][y], pos->board, index_x, index_y, x, y, color);
                        index++;
                    }
                    if (pos->board[x][y] != __)
                    {
                        break;
                    }
                }
                // down right
                for (int j = 1; j <= 7; j++)
                {
                    x = index_x + j;
                    y = index_y + j;
                    if (x > 7 || y > 7)
                    {
                        break;
                    }
                    if (pos->board[x][y] == __)
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(pos, &all_moves[index], origin, x, y, ply, color);
                        index++;
                    }
                    else if (isBlackPiece(pos->board[x][y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        all_moves[index].promotion = ' ';
                        sort[index] = capMove_score(wB, pos->board[x][y], pos->board, index_x, index_y, x, y, color);
                        index++;
                    }
                    if (pos->board[x][y] != __)
                    {
                        break;
                    }
                }
            }

            // rook
            for (int i = 0; i < pos->piece_count[wR]; i++)
            {
                origin = pos->piece_list[wR][i];
                index_x = origin / 8;
                index_y = origin % 8;
                // up
                for (int j = index_x - 1; j >= 0; j--)
                {
                    if (pos->board[j][index_y] == __)
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * j + index_y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(pos, &all_moves[index], origin, j, index_y, ply, color);
                        index++;
                    }
                    else if (isBlackPiece(pos->board[j][index_y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * j + index_y;
                        all_moves[index].promotion = ' ';
                        sort[index] = capMove_score(wR, pos->board[j][index_y], pos->board, index_x, index_y, j, index_y, color);
                        index++;
                    }
                    if (pos->board[j][index_y] != __)
                    {
                        break;
                    }
                }
                // left
                for (int j = index_y - 1; j >= 0; j--)
                {
                    if (pos->board[index_x][j] == __)
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * index_x + j;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(pos, &all_moves[index], origin, index_x, j, ply, color);
                        index++;
                    }
                    else if (isBlackPiece(pos->board[index_x][j]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * index_x + j;
                        all_moves[index].promotion = ' ';
                        sort[index] = capMove_score(wR, pos->board[index_x][j], pos->board, index_x, index_y, index_x, j, color);
                        index++;
                    }
                    if (pos->board[index_x][j] != __)
                    {
                        break;
                    }
                }
                // right
                for (int j = index_y + 1; j <= 7; j++)
                {
                    if (pos->board[index_x][j] == __)
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * index_x + j;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(pos, &all_moves[index], origin, index_x, j, ply, color);
                        index++;
                    }
                    else if (isBlackPiece(pos->board[index_x][j]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * index_x + j;
                        all_moves[index].promotion = ' ';
                        sort[index] = capMove_score(wR, pos->board[index_x][j], pos->board, index_x, index_y, index_x, j, color);
                        index++;
                    }
                    if (pos->board[index_x][j] != __)
                    {
                        break;
                    }
                }
                // down
                for (int j = index_x + 1; j <= 7; j++)
                {
                    if (pos->board[j][index_y] == __)
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * j + index_y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(pos, &all_moves[index], origin, j, index_y, ply, color);
                        index++;
                    }
                    else if (isBlackPiece(pos->board[j][index_y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * j + index_y;
                        all_moves[index].promotion = ' ';
                        sort[index] = capMove_score(wR, pos->board[j][index_y], pos->board, index_x, index_y, j, index_y, color);
                        index++;
                    }
                    if (pos->board[j][index_y] != __)
                    {
                        break;
                    }
                }
            }

            // queen
            for (int i = 0; i < pos->piece_count[wQ]; i++)
            {
                origin = pos->piece_list[wQ][i];
                index_x = origin / 8;
                index_y = origin % 8;
                // up left
                for (int j = 1; j <= 7; j++)
                {
                    x = index_x - j;
                    y = index_y - j;
                    if (x < 0 || y < 0)
                    {
                        break;
                    }
                    if (pos->board[x][y] == __)
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(pos, &all_moves[index], origin, x, y, ply, color);
                        index++;
                    }
                    else if (isBlackPiece(pos->board[x][y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        all_moves[index].promotion = ' ';
                        sort[index] = capMove_score(wQ, pos->board[x][y], pos->board, index_x, index_y, x, y, color);
                        index++;
                    }
                    if (pos->board[x][y] != __)
                    {
                        break;
                    }
                }
                // up
                for (int j = index_x - 1; j >= 0; j--)
                {
                    if (pos->board[j][index_y] == __)
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * j + index_y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(pos, &all_moves[index], origin, j, index_y, ply, color);
                        index++;
                    }
                    else if (isBlackPiece(pos->board[j][index_y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * j + index_y;
                        all_moves[index].promotion = ' ';
                        sort[index] = capMove_score(wQ, pos->board[j][index_y], pos->board, index_x, index_y, j, index_y, color);
                        index++;
                    }
                    if (pos->board[j][index_y] != __)
                    {
                        break;
                    }
                }
                // up right
                for (int j = 1; j <= 7; j++)
                {
                    x = index_x - j;
                    y = index_y + j;
                    if (x < 0 || y > 7)
                    {
                        break;
                    }
                    if (pos->board[x][y] == __)
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(pos, &all_moves[index], origin, x, y, ply, color);
                        index++;
                    }
                    else if (isBlackPiece(pos->board[x][y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        all_moves[index].promotion = ' ';
                        sort[index] = capMove_score(wQ, pos->board[x][y], pos->board, index_x, index_y, x, y, color);
                        index++;
                    }
                    if (pos->board[x][y] != __)
                    {
                        break;
                    }
                }
                // left
                for (int j = index_y - 1; j >= 0; j--)
                {
                    if (pos->board[index_x][j] == __)
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * index_x + j;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(pos, &all_moves[index], origin, index_x, j, ply, color);
                        index++;
                    }
                    else if (isBlackPiece(pos->board[index_x][j]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * index_x + j;
                        all_moves[index].promotion = ' ';
                        sort[index] = capMove_score(wQ, pos->board[index_x][j], pos->board, index_x, index_y, index_x, j, color);
                        index++;
                    }
                    if (pos->board[index_x][j] != __)
                    {
                        break;
                    }
                }
                // right
                for (int j = index_y + 1; j <= 7; j++)
                {
                    if (pos->board[index_x][j] == __)
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * index_x + j;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(pos, &all_moves[index], origin, index_x, j, ply, color);
                        index++;
                    }
                    else if (isBlackPiece(pos->board[index_x][j]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * index_x + j;
                        all_moves[index].promotion = ' ';
                        sort[index] = capMove_score(wQ, pos->board[index_x][j], pos->board, index_x, index_y, index_x, j, color);
                        index++;
                    }
                    if (pos->board[index_x][j] != __)
                    {
                        break;
                    }
                }
                // down left
                for (int j = 1; j <= 7; j++)
                {
                    x = index_x + j;
                    y = index_y - j;
                    if (x > 7 || y < 0)
                    {
                        break;
                    }
                    if (pos->board[x][y] == __)
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(pos, &all_moves[index], origin, x, y, ply, color);
                        index++;
                    }
                    else if (isBlackPiece(pos->board[x][y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        all_moves[index].promotion = ' ';
                        sort[index] = capMove_score(wQ, pos->board[x][y], pos->board, index_x, index_y, x, y, color);
                        index++;
                    }
                    if (pos->board[x][y] != __)
                    {
                        break;
                    }
                }
                // down
                for (int j = index_x + 1; j <= 7; j++)
                {
                    if (pos->board[j][index_y] == __)
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * j + index_y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(pos, &all_moves[index], origin, j, index_y, ply, color);
                        index++;
                    }
                    else if (isBlackPiece(pos->board[j][index_y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * j + index_y;
                        all_moves[index].promotion = ' ';
                        sort[index] = capMove_score(wQ, pos->board[j][index_y], pos->board, index_x, index_y, j, index_y, color);
                        index++;
                    }
                    if (pos->board[j][index_y] != __)
                    {
                        break;
                    }
                }
                // down right
                for (int j = 1; j <= 7; j++)
                {
                    x = index_x + j;
                    y = index_y + j;
                    if (x > 7 || y > 7)
                    {
                        break;
                    }
                    if (pos->board[x][y] == __)
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        all_moves[index].promotion = ' ';
                        sort[index] = quietMove_score(pos, &all_moves[index], origin, x, y, ply, color);
                        index++;
                    }
                    else if (isBlackPiece(pos->board[x][y]))
                    {
                        all_moves[index].from = origin;
                        all_moves[index].to = 8 * x + y;
                        all_moves[index].promotion = ' ';
                        sort[index] = capMove_score(wQ, pos->board[x][y], pos->board, index_x, index_y, x, y, color);
                        index++;
                    }
                    if (pos->board[x][y] != __)
                    {
                        break;
                    }
                }
            }

            // king
            origin = pos->piece_list[wK][0];
            index_x = origin / 8;
            index_y = origin % 8;
            // castling
            if (origin == e1)
            {
                if (checkMove_wkingside(pos))
                {
                    all_moves[index].from = origin;
                    all_moves[index].to = g1;
                    all_moves[index].promotion = ' ';
                    sort[index] = CASTLING;
                    index++;
                }
                if (checkMove_wqueenside(pos))
                {
                    all_moves[index].from = origin;
                    all_moves[index].to = c1;
                    all_moves[index].promotion = ' ';
                    sort[index] = CASTLING;
                    index++;
                }
            }
            for (int j = 0; j < 8; j++)
            {
                x = index_x + king_moves_x[j];
                y = index_y + king_moves_y[j];
                if (x & 8 || y & 8) // skip when out of board
                {
                    continue;
                }
                if (pos->board[x][y] == __)
                {
                    all_moves[index].from = origin;
                    all_moves[index].to = 8 * x + y;
                    all_moves[index].promotion = ' ';
                    sort[index] = quietMove_score(pos, &all_moves[index], origin, x, y, ply, color);
                    index++;
                }
                else if (isBlackPiece(pos->board[x][y]))
                {
                    all_moves[index].from = origin;
                    all_moves[index].to = 8 * x + y;
                    all_moves[index].promotion = ' ';
                    sort[index] = capMove_score(wK, pos->board[x][y], pos->board, index_x, index_y, x, y, color);
                    index++;
                }
            }
            break;
        }
    }

    return index;
}