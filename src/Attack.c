#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>
#include "attack.h"
#include "board.h"

// check if a side is in check
// return 1 if yes
// return 0 if no
// color = 1: black king
// color = -1: white king
int ifCheck(BOARD *pos, int color)
{
    int king_x = -1;
    int king_y = -1;

    switch (color)
    {
        case 1:
        {
            king_x = pos->piece_list[bK][0] / 8;
            king_y = pos->piece_list[bK][0] % 8;

            // down left
            for (int i = 1; i <= 7; i++)
            {
                if (king_x + i > 7 || king_y - i < 0)
                {
                    break;
                }
                if (pos->board[king_x + i][king_y - i] != __)
                {
                    if (i == 1 && (pos->board[king_x + i][king_y - i] == wP || pos->board[king_x + i][king_y - i] == wK))
                    {
                        return 1;
                    }
                    if (pos->board[king_x + i][king_y - i] == wB || pos->board[king_x + i][king_y - i] == wQ)
                    {
                        return 1;
                    }
                    break;
                }
            }

            // down right
            for (int i = 1; i <= 7; i++)
            {
                if (king_x + i > 7 || king_y + i > 7)
                {
                    break;
                }
                if (pos->board[king_x + i][king_y + i] != __)
                {
                    if (i == 1 && (pos->board[king_x + i][king_y + i] == wP || pos->board[king_x + i][king_y + i] == wK))
                    {
                        return 1;
                    }
                    if (pos->board[king_x + i][king_y + i] == wB || pos->board[king_x + i][king_y + i] == wQ)
                    {
                        return 1;
                    }
                    break;
                }
            }

            // knight attacks
            if (king_x + 2 <= 7 && king_y + 1 <= 7)
            {
                if (pos->board[king_x + 2][king_y + 1] == wN)
                    return 1;
            }
            if (king_x + 2 <= 7 && king_y - 1 >= 0)
            {
                if (pos->board[king_x + 2][king_y - 1] == wN)
                    return 1;
            }
            if (king_x + 1 <= 7 && king_y + 2 <= 7)
            {
                if (pos->board[king_x + 1][king_y + 2] == wN)
                    return 1;
            }
            if (king_x + 1 <= 7 && king_y - 2 >= 0)
            {
                if (pos->board[king_x + 1][king_y - 2] == wN)
                    return 1;
            }
            if (king_x - 2 >= 0 && king_y + 1 <= 7)
            {
                if (pos->board[king_x - 2][king_y + 1] == wN)
                    return 1;
            }
            if (king_x - 2 >= 0 && king_y - 1 >= 0)
            {
                if (pos->board[king_x - 2][king_y - 1] == wN)
                    return 1;
            }
            if (king_x - 1 >= 0 && king_y + 2 <= 7)
            {
                if (pos->board[king_x - 1][king_y + 2] == wN)
                    return 1;
            }
            if (king_x - 1 >= 0 && king_y - 2 >= 0)
            {
                if (pos->board[king_x - 1][king_y - 2] == wN)
                    return 1;
            }

            // down
            for (int i = king_x + 1; i <= 7; i++)
            {
                if (pos->board[i][king_y] != __)
                {
                    if (i == king_x + 1 && pos->board[i][king_y] == wK)
                    {
                        return 1;
                    }
                    if (pos->board[i][king_y] == wR || pos->board[i][king_y] == wQ)
                    {
                        return 1;
                    }
                    break;
                }
            }

            // left
            for (int i = king_y - 1; i >= 0; i--)
            {
                if (pos->board[king_x][i] != __)
                {
                    if (i == king_y - 1 && pos->board[king_x][i] == wK)
                    {
                        return 1;
                    }
                    if (pos->board[king_x][i] == wR || pos->board[king_x][i] == wQ)
                    {
                        return 1;
                    }
                    break;
                }
            }

            // right
            for (int i = king_y + 1; i <= 7; i++)
            {
                if (pos->board[king_x][i] != __)
                {
                    if (i == king_y + 1 && pos->board[king_x][i] == wK)
                    {
                        return 1;
                    }
                    if (pos->board[king_x][i] == wR || pos->board[king_x][i] == wQ)
                    {
                        return 1;
                    }
                    break;
                }
            }

            // up
            for (int i = king_x - 1; i >= 0; i--)
            {
                if (pos->board[i][king_y] != __)
                {
                    if (i == king_x - 1 && pos->board[i][king_y] == wK)
                    {
                        return 1;
                    }
                    if (pos->board[i][king_y] == wR || pos->board[i][king_y] == wQ)
                    {
                        return 1;
                    }
                    break;
                }
            }

            // up left
            for (int i = 1; i <= 7; i++)
            {
                if (king_x - i < 0 || king_y - i < 0)
                {
                    break;
                }
                if (pos->board[king_x - i][king_y - i] != __)
                {
                    if (i == 1 && pos->board[king_x - i][king_y - i] == wK)
                    {
                        return 1;
                    }
                    if (pos->board[king_x - i][king_y - i] == wB || pos->board[king_x - i][king_y - i] == wQ)
                    {
                        return 1;
                    }
                    break;
                }
            }

            // up right
            for (int i = 1; i <= 7; i++)
            {
                if (king_x - i < 0 || king_y + i > 7)
                {
                    break;
                }
                if (pos->board[king_x - i][king_y + i] != __)
                {
                    if (i == 1 && pos->board[king_x - i][king_y + i] == wK)
                    {
                        return 1;
                    }
                    if (pos->board[king_x - i][king_y + i] == wB || pos->board[king_x - i][king_y + i] == wQ)
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
            king_x = pos->piece_list[wK][0] / 8;
            king_y = pos->piece_list[wK][0] % 8;

            // up left
            for (int i = 1; i <= 7; i++)
            {
                if (king_x - i < 0 || king_y - i < 0)
                {
                    break;
                }
                if (pos->board[king_x - i][king_y - i] != __)
                {
                    if (i == 1 && (pos->board[king_x - i][king_y - i] == bP || pos->board[king_x - i][king_y - i] == bK))
                    {
                        return 1;
                    }
                    if (pos->board[king_x - i][king_y - i] == bB || pos->board[king_x - i][king_y - i] == bQ)
                    {
                        return 1;
                    }
                    break;
                }
            }

            // up right
            for (int i = 1; i <= 7; i++)
            {
                if (king_x - i < 0 || king_y + i > 7)
                {
                    break;
                }
                if (pos->board[king_x - i][king_y + i] != __)
                {
                    if (i == 1 && (pos->board[king_x - i][king_y + i] == bP || pos->board[king_x - i][king_y + i] == bK))
                    {
                        return 1;
                    }
                    if (pos->board[king_x - i][king_y + i] == bB || pos->board[king_x - i][king_y + i] == bQ)
                    {
                        return 1;
                    }
                    break;
                }
            }

            // knight attacks
            if (king_x - 2 >= 0 && king_y + 1 <= 7)
            {
                if (pos->board[king_x - 2][king_y + 1] == bN)
                    return 1;
            }
            if (king_x - 2 >= 0 && king_y - 1 >= 0)
            {
                if (pos->board[king_x - 2][king_y - 1] == bN)
                    return 1;
            }
            if (king_x - 1 >= 0 && king_y + 2 <= 7)
            {
                if (pos->board[king_x - 1][king_y + 2] == bN)
                    return 1;
            }
            if (king_x - 1 >= 0 && king_y - 2 >= 0)
            {
                if (pos->board[king_x - 1][king_y - 2] == bN)
                    return 1;
            }
            if (king_x + 2 <= 7 && king_y + 1 <= 7)
            {
                if (pos->board[king_x + 2][king_y + 1] == bN)
                    return 1;
            }
            if (king_x + 2 <= 7 && king_y - 1 >= 0)
            {
                if (pos->board[king_x + 2][king_y - 1] == bN)
                    return 1;
            }
            if (king_x + 1 <= 7 && king_y + 2 <= 7)
            {
                if (pos->board[king_x + 1][king_y + 2] == bN)
                    return 1;
            }
            if (king_x + 1 <= 7 && king_y - 2 >= 0)
            {
                if (pos->board[king_x + 1][king_y - 2] == bN)
                    return 1;
            }

            // up
            for (int i = king_x - 1; i >= 0; i--)
            {
                if (pos->board[i][king_y] != __)
                {
                    if (i == king_x - 1 && pos->board[i][king_y] == bK)
                    {
                        return 1;
                    }
                    if (pos->board[i][king_y] == bR || pos->board[i][king_y] == bQ)
                    {
                        return 1;
                    }
                    break;
                }
            }

            // left
            for (int i = king_y - 1; i >= 0; i--)
            {
                if (pos->board[king_x][i] != __)
                {
                    if (i == king_y - 1 && pos->board[king_x][i] == bK)
                    {
                        return 1;
                    }
                    if (pos->board[king_x][i] == bR || pos->board[king_x][i] == bQ)
                    {
                        return 1;
                    }
                    break;
                }
            }

            // right
            for (int i = king_y + 1; i <= 7; i++)
            {
                if (pos->board[king_x][i] != __)
                {
                    if (i == king_y + 1 && pos->board[king_x][i] == bK)
                    {
                        return 1;
                    }
                    if (pos->board[king_x][i] == bR || pos->board[king_x][i] == bQ)
                    {
                        return 1;
                    }
                    break;
                }
            }

            // down
            for (int i = king_x + 1; i <= 7; i++)
            {
                if (pos->board[i][king_y] != __)
                {
                    if (i == king_x + 1 && pos->board[i][king_y] == bK)
                    {
                        return 1;
                    }
                    if (pos->board[i][king_y] == bR || pos->board[i][king_y] == bQ)
                    {
                        return 1;
                    }
                    break;
                }
            }

            // down left
            for (int i = 1; i <= 7; i++)
            {
                if (king_x + i > 7 || king_y - i < 0)
                {
                    break;
                }
                if (pos->board[king_x + i][king_y - i] != __)
                {
                    if (i == 1 && pos->board[king_x + i][king_y - i] == bK)
                    {
                        return 1;
                    }
                    if (pos->board[king_x + i][king_y - i] == bB || pos->board[king_x + i][king_y - i] == bQ)
                    {
                        return 1;
                    }
                    break;
                }
            }

            // down right
            for (int i = 1; i <= 7; i++)
            {
                if (king_x + i > 7 || king_y + i > 7)
                {
                    break;
                }
                if (pos->board[king_x + i][king_y + i] != __)
                {
                    if (i == 1 && pos->board[king_x + i][king_y + i] == bK)
                    {
                        return 1;
                    }
                    if (pos->board[king_x + i][king_y + i] == bB || pos->board[king_x + i][king_y + i] == bQ)
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

// check if a piece in a given position is threatened
// x and y are the rank and file of the position
// return 1 if yes
// return 0 if no
// color = 1: threatened by white pieces
// color = -1: threatened by black pieces
int isThreatened(unsigned char board[8][8], int x, int y, int color)
{
    switch (color)
    {
        case 1:
        {
            // down left
            for (int i = 1; i <= 7; i++)
            {
                if (x + i > 7 || y - i < 0)
                {
                    break;
                }
                if (board[x + i][y - i] != __)
                {
                    if (i == 1 && (board[x + i][y - i] == wP || board[x + i][y - i] == wK))
                    {
                        return 1;
                    }
                    if (board[x + i][y - i] == wB || board[x + i][y - i] == wQ)
                    {
                        return 1;
                    }
                    break;
                }
            }

            // down right
            for (int i = 1; i <= 7; i++)
            {
                if (x + i > 7 || y + i > 7)
                {
                    break;
                }
                if (board[x + i][y + i] != __)
                {
                    if (i == 1 && (board[x + i][y + i] == wP || board[x + i][y + i] == wK))
                    {
                        return 1;
                    }
                    if (board[x + i][y + i] == wB || board[x + i][y + i] == wQ)
                    {
                        return 1;
                    }
                    break;
                }
            }

            // knight attacks
            if (x - 2 >= 0 && y + 1 <= 7)
            {
                if (board[x - 2][y + 1] == wN)
                    return 1;
            }
            if (x - 2 >= 0 && y - 1 >= 0)
            {
                if (board[x - 2][y - 1] == wN)
                    return 1;
            }
            if (x - 1 >= 0 && y + 2 <= 7)
            {
                if (board[x - 1][y + 2] == wN)
                    return 1;
            }
            if (x - 1 >= 0 && y - 2 >= 0)
            {
                if (board[x - 1][y - 2] == wN)
                    return 1;
            }
            if (x + 2 <= 7 && y + 1 <= 7)
            {
                if (board[x + 2][y + 1] == wN)
                    return 1;
            }
            if (x + 2 <= 7 && y - 1 >= 0)
            {
                if (board[x + 2][y - 1] == wN)
                    return 1;
            }
            if (x + 1 <= 7 && y + 2 <= 7)
            {
                if (board[x + 1][y + 2] == wN)
                    return 1;
            }
            if (x + 1 <= 7 && y - 2 >= 0)
            {
                if (board[x + 1][y - 2] == wN)
                    return 1;
            }

            // left
            for (int i = y - 1; i >= 0; i--)
            {
                if (board[x][i] != __)
                {
                    if (i == y - 1 && board[x][i] == wK)
                    {
                        return 1;
                    }
                    if (board[x][i] == wR || board[x][i] == wQ)
                    {
                        return 1;
                    }
                    break;
                }
            }

            // right
            for (int i = y + 1; i <= 7; i++)
            {
                if (board[x][i] != __)
                {
                    if (i == y + 1 && board[x][i] == wK)
                    {
                        return 1;
                    }
                    if (board[x][i] == wR || board[x][i] == wQ)
                    {
                        return 1;
                    }
                    break;
                }
            }

            // up
            for (int i = x - 1; i >= 0; i--)
            {
                if (board[i][y] != __)
                {
                    if (i == x - 1 && board[i][y] == wK)
                    {
                        return 1;
                    }
                    if (board[i][y] == wR || board[i][y] == wQ)
                    {
                        return 1;
                    }
                    break;
                }
            }

            // down
            for (int i = x + 1; i <= 7; i++)
            {
                if (board[i][y] != __)
                {
                    if (i == x + 1 && board[i][y] == wK)
                    {
                        return 1;
                    }
                    if (board[i][y] == wR || board[i][y] == wQ)
                    {
                        return 1;
                    }
                    break;
                }
            }

            // up left
            for (int i = 1; i <= 7; i++)
            {
                if (x - i < 0 || y - i < 0)
                {
                    break;
                }
                if (board[x - i][y - i] != __)
                {
                    if (i == 1 && board[x - i][y - i] == wK)
                    {
                        return 1;
                    }
                    if (board[x - i][y - i] == wB || board[x - i][y - i] == wQ)
                    {
                        return 1;
                    }
                    break;
                }
            }

            // up right
            for (int i = 1; i <= 7; i++)
            {
                if (x - i < 0 || y + i > 7)
                {
                    break;
                }
                if (board[x - i][y + i] != __)
                {
                    if (i == 1 && board[x - i][y + i] == wK)
                    {
                        return 1;
                    }
                    if (board[x - i][y + i] == wB || board[x - i][y + i] == wQ)
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
            // up left
            for (int i = 1; i <= 7; i++)
            {
                if (x - i < 0 || y - i < 0)
                {
                    break;
                }
                if (board[x - i][y - i] != __)
                {
                    if (i == 1 && (board[x - i][y - i] == bP || board[x - i][y - i] == bK))
                    {
                        return 1;
                    }
                    if (board[x - i][y - i] == bB || board[x - i][y - i] == bQ)
                    {
                        return 1;
                    }
                    break;
                }
            }

            // up right
            for (int i = 1; i <= 7; i++)
            {
                if (x - i < 0 || y + i > 7)
                {
                    break;
                }
                if (board[x - i][y + i] != __)
                {
                    if (i == 1 && (board[x - i][y + i] == bP || board[x - i][y + i] == bK))
                    {
                        return 1;
                    }
                    if (board[x - i][y + i] == bB || board[x - i][y + i] == bQ)
                    {
                        return 1;
                    }
                    break;
                }
            }

            // knight attacks
            if (x - 2 >= 0 && y + 1 <= 7)
            {
                if (board[x - 2][y + 1] == bN)
                    return 1;
            }
            if (x - 2 >= 0 && y - 1 >= 0)
            {
                if (board[x - 2][y - 1] == bN)
                    return 1;
            }
            if (x - 1 >= 0 && y + 2 <= 7)
            {
                if (board[x - 1][y + 2] == bN)
                    return 1;
            }
            if (x - 1 >= 0 && y - 2 >= 0)
            {
                if (board[x - 1][y - 2] == bN)
                    return 1;
            }
            if (x + 2 <= 7 && y + 1 <= 7)
            {
                if (board[x + 2][y + 1] == bN)
                    return 1;
            }
            if (x + 2 <= 7 && y - 1 >= 0)
            {
                if (board[x + 2][y - 1] == bN)
                    return 1;
            }
            if (x + 1 <= 7 && y + 2 <= 7)
            {
                if (board[x + 1][y + 2] == bN)
                    return 1;
            }
            if (x + 1 <= 7 && y - 2 >= 0)
            {
                if (board[x + 1][y - 2] == bN)
                    return 1;
            }

            // left
            for (int i = y - 1; i >= 0; i--)
            {
                if (board[x][i] != __)
                {
                    if (i == y - 1 && board[x][i] == bK)
                    {
                        return 1;
                    }
                    if (board[x][i] == bR || board[x][i] == bQ)
                    {
                        return 1;
                    }
                    break;
                }
            }

            // right
            for (int i = y + 1; i <= 7; i++)
            {
                if (board[x][i] != __)
                {
                    if (i == y + 1 && board[x][i] == bK)
                    {
                        return 1;
                    }
                    if (board[x][i] == bR || board[x][i] == bQ)
                    {
                        return 1;
                    }
                    break;
                }
            }

            // up
            for (int i = x - 1; i >= 0; i--)
            {
                if (board[i][y] != __)
                {
                    if (i == x - 1 && board[i][y] == bK)
                    {
                        return 1;
                    }
                    if (board[i][y] == bR || board[i][y] == bQ)
                    {
                        return 1;
                    }
                    break;
                }
            }

            // down
            for (int i = x + 1; i <= 7; i++)
            {
                if (board[i][y] != __)
                {
                    if (i == x + 1 && board[i][y] == bK)
                    {
                        return 1;
                    }
                    if (board[i][y] == bR || board[i][y] == bQ)
                    {
                        return 1;
                    }
                    break;
                }
            }

            // down left
            for (int i = 1; i <= 7; i++)
            {
                if (x + i > 7 || y - i < 0)
                {
                    break;
                }
                if (board[x + i][y - i] != __)
                {
                    if (i == 1 && board[x + i][y - i] == bK)
                    {
                        return 1;
                    }
                    if (board[x + i][y - i] == bB || board[x + i][y - i] == bQ)
                    {
                        return 1;
                    }
                    break;
                }
            }
            
            // down right
            for (int i = 1; i <= 7; i++)
            {
                if (x + i > 7 || y + i > 7)
                {
                    break;
                }
                if (board[x + i][y + i] != __)
                {
                    if (i == 1 && board[x + i][y + i] == bK)
                    {
                        return 1;
                    }
                    if (board[x + i][y + i] == bB || board[x + i][y + i] == bQ)
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