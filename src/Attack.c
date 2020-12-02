#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>
#include "Attack.h"
#include "Board.h"

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
                if (pos->board[king_x + i][king_y - i] != ' ')
                {
                    if (i == 1 && (pos->board[king_x + i][king_y - i] == 'P' || pos->board[king_x + i][king_y - i] == 'K'))
                    {
                        return 1;
                    }
                    if (pos->board[king_x + i][king_y - i] == 'B' || pos->board[king_x + i][king_y - i] == 'Q')
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
                if (pos->board[king_x + i][king_y + i] != ' ')
                {
                    if (i == 1 && (pos->board[king_x + i][king_y + i] == 'P' || pos->board[king_x + i][king_y + i] == 'K'))
                    {
                        return 1;
                    }
                    if (pos->board[king_x + i][king_y + i] == 'B' || pos->board[king_x + i][king_y + i] == 'Q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            // knight attacks
            if (king_x + 2 <= 7 && king_y + 1 <= 7)
            {
                if (pos->board[king_x + 2][king_y + 1] == 'N')
                    return 1;
            }
            if (king_x + 2 <= 7 && king_y - 1 >= 0)
            {
                if (pos->board[king_x + 2][king_y - 1] == 'N')
                    return 1;
            }
            if (king_x + 1 <= 7 && king_y + 2 <= 7)
            {
                if (pos->board[king_x + 1][king_y + 2] == 'N')
                    return 1;
            }
            if (king_x + 1 <= 7 && king_y - 2 >= 0)
            {
                if (pos->board[king_x + 1][king_y - 2] == 'N')
                    return 1;
            }
            if (king_x - 2 >= 0 && king_y + 1 <= 7)
            {
                if (pos->board[king_x - 2][king_y + 1] == 'N')
                    return 1;
            }
            if (king_x - 2 >= 0 && king_y - 1 >= 0)
            {
                if (pos->board[king_x - 2][king_y - 1] == 'N')
                    return 1;
            }
            if (king_x - 1 >= 0 && king_y + 2 <= 7)
            {
                if (pos->board[king_x - 1][king_y + 2] == 'N')
                    return 1;
            }
            if (king_x - 1 >= 0 && king_y - 2 >= 0)
            {
                if (pos->board[king_x - 1][king_y - 2] == 'N')
                    return 1;
            }
            // down
            for (int i = king_x + 1; i <= 7; i++)
            {
                if (pos->board[i][king_y] != ' ')
                {
                    if (i == king_x + 1 && pos->board[i][king_y] == 'K')
                    {
                        return 1;
                    }
                    if (pos->board[i][king_y] == 'R' || pos->board[i][king_y] == 'Q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            // left
            for (int i = king_y - 1; i >= 0; i--)
            {
                if (pos->board[king_x][i] != ' ')
                {
                    if (i == king_y - 1 && pos->board[king_x][i] == 'K')
                    {
                        return 1;
                    }
                    if (pos->board[king_x][i] == 'R' || pos->board[king_x][i] == 'Q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            // right
            for (int i = king_y + 1; i <= 7; i++)
            {
                if (pos->board[king_x][i] != ' ')
                {
                    if (i == king_y + 1 && pos->board[king_x][i] == 'K')
                    {
                        return 1;
                    }
                    if (pos->board[king_x][i] == 'R' || pos->board[king_x][i] == 'Q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            // up
            for (int i = king_x - 1; i >= 0; i--)
            {
                if (pos->board[i][king_y] != ' ')
                {
                    if (i == king_x - 1 && pos->board[i][king_y] == 'K')
                    {
                        return 1;
                    }
                    if (pos->board[i][king_y] == 'R' || pos->board[i][king_y] == 'Q')
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
                if (pos->board[king_x - i][king_y - i] != ' ')
                {
                    if (i == 1 && pos->board[king_x - i][king_y - i] == 'K')
                    {
                        return 1;
                    }
                    if (pos->board[king_x - i][king_y - i] == 'B' || pos->board[king_x - i][king_y - i] == 'Q')
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
                if (pos->board[king_x - i][king_y + i] != ' ')
                {
                    if (i == 1 && pos->board[king_x - i][king_y + i] == 'K')
                    {
                        return 1;
                    }
                    if (pos->board[king_x - i][king_y + i] == 'B' || pos->board[king_x - i][king_y + i] == 'Q')
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
                if (pos->board[king_x - i][king_y - i] != ' ')
                {
                    if (i == 1 && (pos->board[king_x - i][king_y - i] == 'p' || pos->board[king_x - i][king_y - i] == 'k'))
                    {
                        return 1;
                    }
                    if (pos->board[king_x - i][king_y - i] == 'b' || pos->board[king_x - i][king_y - i] == 'q')
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
                if (pos->board[king_x - i][king_y + i] != ' ')
                {
                    if (i == 1 && (pos->board[king_x - i][king_y + i] == 'p' || pos->board[king_x - i][king_y + i] == 'k'))
                    {
                        return 1;
                    }
                    if (pos->board[king_x - i][king_y + i] == 'b' || pos->board[king_x - i][king_y + i] == 'q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            // knight attacks
            if (king_x - 2 >= 0 && king_y + 1 <= 7)
            {
                if (pos->board[king_x - 2][king_y + 1] == 'n')
                    return 1;
            }
            if (king_x - 2 >= 0 && king_y - 1 >= 0)
            {
                if (pos->board[king_x - 2][king_y - 1] == 'n')
                    return 1;
            }
            if (king_x - 1 >= 0 && king_y + 2 <= 7)
            {
                if (pos->board[king_x - 1][king_y + 2] == 'n')
                    return 1;
            }
            if (king_x - 1 >= 0 && king_y - 2 >= 0)
            {
                if (pos->board[king_x - 1][king_y - 2] == 'n')
                    return 1;
            }
            if (king_x + 2 <= 7 && king_y + 1 <= 7)
            {
                if (pos->board[king_x + 2][king_y + 1] == 'n')
                    return 1;
            }
            if (king_x + 2 <= 7 && king_y - 1 >= 0)
            {
                if (pos->board[king_x + 2][king_y - 1] == 'n')
                    return 1;
            }
            if (king_x + 1 <= 7 && king_y + 2 <= 7)
            {
                if (pos->board[king_x + 1][king_y + 2] == 'n')
                    return 1;
            }
            if (king_x + 1 <= 7 && king_y - 2 >= 0)
            {
                if (pos->board[king_x + 1][king_y - 2] == 'n')
                    return 1;
            }
            // up
            for (int i = king_x - 1; i >= 0; i--)
            {
                if (pos->board[i][king_y] != ' ')
                {
                    if (i == king_x - 1 && pos->board[i][king_y] == 'k')
                    {
                        return 1;
                    }
                    if (pos->board[i][king_y] == 'r' || pos->board[i][king_y] == 'q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            // left
            for (int i = king_y - 1; i >= 0; i--)
            {
                if (pos->board[king_x][i] != ' ')
                {
                    if (i == king_y - 1 && pos->board[king_x][i] == 'k')
                    {
                        return 1;
                    }
                    if (pos->board[king_x][i] == 'r' || pos->board[king_x][i] == 'q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            // right
            for (int i = king_y + 1; i <= 7; i++)
            {
                if (pos->board[king_x][i] != ' ')
                {
                    if (i == king_y + 1 && pos->board[king_x][i] == 'k')
                    {
                        return 1;
                    }
                    if (pos->board[king_x][i] == 'r' || pos->board[king_x][i] == 'q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            // down
            for (int i = king_x + 1; i <= 7; i++)
            {
                if (pos->board[i][king_y] != ' ')
                {
                    if (i == king_x + 1 && pos->board[i][king_y] == 'k')
                    {
                        return 1;
                    }
                    if (pos->board[i][king_y] == 'r' || pos->board[i][king_y] == 'q')
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
                if (pos->board[king_x + i][king_y - i] != ' ')
                {
                    if (i == 1 && pos->board[king_x + i][king_y - i] == 'k')
                    {
                        return 1;
                    }
                    if (pos->board[king_x + i][king_y - i] == 'b' || pos->board[king_x + i][king_y - i] == 'q')
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
                if (pos->board[king_x + i][king_y + i] != ' ')
                {
                    if (i == 1 && pos->board[king_x + i][king_y + i] == 'k')
                    {
                        return 1;
                    }
                    if (pos->board[king_x + i][king_y + i] == 'b' || pos->board[king_x + i][king_y + i] == 'q')
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
// x and y are the indices of the position
// return 1 if yes
// return 0 if no
// color = 1: threatened by white pieces
// color = -1: threatened by black pieces
int isThreatened(char board[8][8], int x, int y, int color)
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
                if (board[x + i][y - i] != ' ')
                {
                    if (i == 1 && (board[x + i][y - i] == 'P' || board[x + i][y - i] == 'K'))
                    {
                        return 1;
                    }
                    if (board[x + i][y - i] == 'B' || board[x + i][y - i] == 'Q')
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
                if (board[x + i][y + i] != ' ')
                {
                    if (i == 1 && (board[x + i][y + i] == 'P' || board[x + i][y + i] == 'K'))
                    {
                        return 1;
                    }
                    if (board[x + i][y + i] == 'B' || board[x + i][y + i] == 'Q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            // knight attacks
            if (x - 2 >= 0 && y + 1 <= 7)
            {
                if (board[x - 2][y + 1] == 'N')
                    return 1;
            }
            if (x - 2 >= 0 && y - 1 >= 0)
            {
                if (board[x - 2][y - 1] == 'N')
                    return 1;
            }
            if (x - 1 >= 0 && y + 2 <= 7)
            {
                if (board[x - 1][y + 2] == 'N')
                    return 1;
            }
            if (x - 1 >= 0 && y - 2 >= 0)
            {
                if (board[x - 1][y - 2] == 'N')
                    return 1;
            }
            if (x + 2 <= 7 && y + 1 <= 7)
            {
                if (board[x + 2][y + 1] == 'N')
                    return 1;
            }
            if (x + 2 <= 7 && y - 1 >= 0)
            {
                if (board[x + 2][y - 1] == 'N')
                    return 1;
            }
            if (x + 1 <= 7 && y + 2 <= 7)
            {
                if (board[x + 1][y + 2] == 'N')
                    return 1;
            }
            if (x + 1 <= 7 && y - 2 >= 0)
            {
                if (board[x + 1][y - 2] == 'N')
                    return 1;
            }
            // left
            for (int i = y - 1; i >= 0; i--)
            {
                if (board[x][i] != ' ')
                {
                    if (i == y - 1 && board[x][i] == 'K')
                    {
                        return 1;
                    }
                    if (board[x][i] == 'R' || board[x][i] == 'Q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            // right
            for (int i = y + 1; i <= 7; i++)
            {
                if (board[x][i] != ' ')
                {
                    if (i == y + 1 && board[x][i] == 'K')
                    {
                        return 1;
                    }
                    if (board[x][i] == 'R' || board[x][i] == 'Q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            // up
            for (int i = x - 1; i >= 0; i--)
            {
                if (board[i][y] != ' ')
                {
                    if (i == x - 1 && board[i][y] == 'K')
                    {
                        return 1;
                    }
                    if (board[i][y] == 'R' || board[i][y] == 'Q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            // down
            for (int i = x + 1; i <= 7; i++)
            {
                if (board[i][y] != ' ')
                {
                    if (i == x + 1 && board[i][y] == 'K')
                    {
                        return 1;
                    }
                    if (board[i][y] == 'R' || board[i][y] == 'Q')
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
                if (board[x - i][y - i] != ' ')
                {
                    if (i == 1 && board[x - i][y - i] == 'K')
                    {
                        return 1;
                    }
                    if (board[x - i][y - i] == 'B' || board[x - i][y - i] == 'Q')
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
                if (board[x - i][y + i] != ' ')
                {
                    if (i == 1 && board[x - i][y + i] == 'K')
                    {
                        return 1;
                    }
                    if (board[x - i][y + i] == 'B' || board[x - i][y + i] == 'Q')
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
                if (board[x - i][y - i] != ' ')
                {
                    if (i == 1 && (board[x - i][y - i] == 'p' || board[x - i][y - i] == 'k'))
                    {
                        return 1;
                    }
                    if (board[x - i][y - i] == 'b' || board[x - i][y - i] == 'q')
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
                if (board[x - i][y + i] != ' ')
                {
                    if (i == 1 && (board[x - i][y + i] == 'p' || board[x - i][y + i] == 'k'))
                    {
                        return 1;
                    }
                    if (board[x - i][y + i] == 'b' || board[x - i][y + i] == 'q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            // knight attacks
            if (x - 2 >= 0 && y + 1 <= 7)
            {
                if (board[x - 2][y + 1] == 'n')
                    return 1;
            }
            if (x - 2 >= 0 && y - 1 >= 0)
            {
                if (board[x - 2][y - 1] == 'n')
                    return 1;
            }
            if (x - 1 >= 0 && y + 2 <= 7)
            {
                if (board[x - 1][y + 2] == 'n')
                    return 1;
            }
            if (x - 1 >= 0 && y - 2 >= 0)
            {
                if (board[x - 1][y - 2] == 'n')
                    return 1;
            }
            if (x + 2 <= 7 && y + 1 <= 7)
            {
                if (board[x + 2][y + 1] == 'n')
                    return 1;
            }
            if (x + 2 <= 7 && y - 1 >= 0)
            {
                if (board[x + 2][y - 1] == 'n')
                    return 1;
            }
            if (x + 1 <= 7 && y + 2 <= 7)
            {
                if (board[x + 1][y + 2] == 'n')
                    return 1;
            }
            if (x + 1 <= 7 && y - 2 >= 0)
            {
                if (board[x + 1][y - 2] == 'n')
                    return 1;
            }
            // left
            for (int i = y - 1; i >= 0; i--)
            {
                if (board[x][i] != ' ')
                {
                    if (i == y - 1 && board[x][i] == 'k')
                    {
                        return 1;
                    }
                    if (board[x][i] == 'r' || board[x][i] == 'q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            // right
            for (int i = y + 1; i <= 7; i++)
            {
                if (board[x][i] != ' ')
                {
                    if (i == y + 1 && board[x][i] == 'k')
                    {
                        return 1;
                    }
                    if (board[x][i] == 'r' || board[x][i] == 'q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            // up
            for (int i = x - 1; i >= 0; i--)
            {
                if (board[i][y] != ' ')
                {
                    if (i == x - 1 && board[i][y] == 'k')
                    {
                        return 1;
                    }
                    if (board[i][y] == 'r' || board[i][y] == 'q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            // down
            for (int i = x + 1; i <= 7; i++)
            {
                if (board[i][y] != ' ')
                {
                    if (i == x + 1 && board[i][y] == 'k')
                    {
                        return 1;
                    }
                    if (board[i][y] == 'r' || board[i][y] == 'q')
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
                if (board[x + i][y - i] != ' ')
                {
                    if (i == 1 && board[x + i][y - i] == 'k')
                    {
                        return 1;
                    }
                    if (board[x + i][y - i] == 'b' || board[x + i][y - i] == 'q')
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
                if (board[x + i][y + i] != ' ')
                {
                    if (i == 1 && board[x + i][y + i] == 'k')
                    {
                        return 1;
                    }
                    if (board[x + i][y + i] == 'b' || board[x + i][y + i] == 'q')
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