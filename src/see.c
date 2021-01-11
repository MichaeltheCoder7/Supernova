#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>
#include "see.h"
#include "move.h"
#include "ordermove.h"

// 1: white attackers
// -1: black attackers
// x and y are the rank and file of the board
// return index 0 - 63 of the smallest attacker on the board
// return -1 if not attacked
static int get_smallest_attacker(unsigned char board[8][8], int x, int y, int color)
{
    int attackers_index[2] = { -2, -2 }; // for queen and king

    switch (color)
    {
        case 1:
        {
            // pawn attack
            if (x + 1 <= 7 && y - 1 >= 0)
            {
                if (board[x + 1][y - 1] == wP)
                {
                    return 8 * (x + 1) + (y - 1);
                }
            }
            if (x + 1 <= 7 && y + 1 <= 7)
            {
                if (board[x + 1][y + 1] == wP)
                {
                    return 8 * (x + 1) + (y + 1);
                }
            }

            // knight attacks
            if (x + 2 <= 7 && y + 1 <= 7)
            {
                if (board[x + 2][y + 1] == wN)
                    return 8 * (x + 2) + (y + 1);
            }
            if (x + 2 <= 7 && y - 1 >= 0)
            {
                if (board[x + 2][y - 1] == wN)
                    return 8 * (x + 2) + (y - 1);
            }
            if (x + 1 <= 7 && y + 2 <= 7)
            {
                if (board[x + 1][y + 2] == wN)
                    return 8 * (x + 1) + (y + 2);
            }
            if (x + 1 <= 7 && y - 2 >= 0)
            {
                if (board[x + 1][y - 2] == wN)
                    return 8 * (x + 1) + (y - 2);
            }
            if (x - 2 >= 0 && y + 1 <= 7)
            {
                if (board[x - 2][y + 1] == wN)
                    return 8 * (x - 2) + (y + 1);
            }
            if (x - 2 >= 0 && y - 1 >= 0)
            {
                if (board[x - 2][y - 1] == wN)
                    return 8 * (x - 2) + (y - 1);
            }
            if (x - 1 >= 0 && y + 2 <= 7)
            {
                if (board[x - 1][y + 2] == wN)
                    return 8 * (x - 1) + (y + 2);
            }
            if (x - 1 >= 0 && y - 2 >= 0)
            {
                if (board[x - 1][y - 2] == wN)
                    return 8 * (x - 1) + (y - 2);
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
                    if (board[x + i][y - i] == wB)
                    {
                        return 8 * (x + i) + (y - i);
                    }
                    else if (board[x + i][y - i] == wQ)
                    {
                        attackers_index[0] = 8 * (x + i) + (y - i);
                    }
                    else if (i == 1 && board[x + i][y - i] == wK)
                    {
                        attackers_index[1] = 8 * (x + i) + (y - i);
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
                    if (board[x + i][y + i] == wB)
                    {
                        return 8 * (x + i) + (y + i);
                    }
                    else if (board[x + i][y + i] == wQ)
                    {
                        attackers_index[0] = 8 * (x + i) + (y + i);
                    }
                    else if (i == 1 && board[x + i][y + i] == wK)
                    {
                        attackers_index[1] = 8 * (x + i) + (y + i);
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
                    if (board[x - i][y - i] == wB)
                    {
                        return 8 * (x - i) + (y - i);
                    }
                    else if (board[x - i][y - i] == wQ)
                    {
                        attackers_index[0] = 8 * (x - i) + (y - i);
                    }
                    else if (i == 1 && board[x - i][y - i] == wK)
                    {
                        attackers_index[1] = 8 * (x - i) + (y - i);
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
                    if (board[x - i][y + i] == wB)
                    {
                        return 8 * (x - i) + (y + i);
                    }
                    else if (board[x - i][y + i] == wQ)
                    {
                        attackers_index[0] = 8 * (x - i) + (y + i);
                    }
                    else if (i == 1 && board[x - i][y + i] == wK)
                    {
                        attackers_index[1] = 8 * (x - i) + (y + i);
                    }
                    break;
                }
            }

            // down
            for (int i = x + 1; i <= 7; i++)
            {
                if (board[i][y] != __)
                {
                    if (board[i][y] == wR)
                    {
                        return 8 * i + y;
                    }
                    else if (board[i][y] == wQ)
                    {
                        attackers_index[0] = 8 * i + y;
                    }
                    else if (i == x + 1 && board[i][y] == wK)
                    {
                        attackers_index[1] = 8 * i + y;
                    }
                    break;
                }
            }

            // left
            for (int i = y - 1; i >= 0; i--)
            {
                if (board[x][i] != __)
                {
                    if (board[x][i] == wR)
                    {
                        return 8 * x + i;
                    }
                    else if (board[x][i] == wQ)
                    {
                        attackers_index[0] = 8 * x + i;
                    }
                    else if (i == y - 1 && board[x][i] == wK)
                    {
                        attackers_index[1] = 8 * x + i;
                    }

                    break;
                }
            }

            // right
            for (int i = y + 1; i <= 7; i++)
            {
                if (board[x][i] != __)
                {
                    if (board[x][i] == wR)
                    {
                        return 8 * x + i;
                    }
                    else if (board[x][i] == wQ)
                    {
                        attackers_index[0] = 8 * x + i;
                    }
                    else if (i == y + 1 && board[x][i] == wK)
                    {
                        attackers_index[1] = 8 * x + i;
                    }
                    break;
                }
            }

            // up
            for (int i = x - 1; i >= 0; i--)
            {
                if (board[i][y] != __)
                {
                    if (board[i][y] == wR)
                    {
                        return 8 * i + y;
                    }
                    else if (board[i][y] == wQ)
                    {
                        attackers_index[0] = 8 * i + y;
                    }
                    else if (i == x - 1 && board[i][y] == wK)
                    {
                        attackers_index[1] = 8 * i + y;
                    }
                    break;
                }
            }
            break;
        }
        case -1:
        {
            // pawn attack
            if (x - 1 >= 0 && y - 1 >= 0)
            {
                if (board[x - 1][y - 1] == bP)
                {
                    return 8 * (x - 1) + (y - 1);
                }
            }
            if (x - 1 >= 0 && y + 1 <= 7)
            {
                if (board[x - 1][y + 1] == bP)
                {
                    return 8 * (x - 1) + (y + 1);
                }
            }

            // knight attacks
            if (x - 2 >= 0 && y + 1 <= 7)
            {
                if (board[x - 2][y + 1] == bN)
                    return 8 * (x - 2) + (y + 1);
            }
            if (x - 2 >= 0 && y - 1 >= 0)
            {
                if (board[x - 2][y - 1] == bN)
                    return 8 * (x - 2) + (y - 1);
            }
            if (x - 1 >= 0 && y + 2 <= 7)
            {
                if (board[x - 1][y + 2] == bN)
                    return 8 * (x - 1) + (y + 2);
            }
            if (x - 1 >= 0 && y - 2 >= 0)
            {
                if (board[x - 1][y - 2] == bN)
                    return 8 * (x - 1) + (y - 2);
            }
            if (x + 2 <= 7 && y + 1 <= 7)
            {
                if (board[x + 2][y + 1] == bN)
                    return 8 * (x + 2) + (y + 1);
            }
            if (x + 2 <= 7 && y - 1 >= 0)
            {
                if (board[x + 2][y - 1] == bN)
                    return 8 * (x + 2) + (y - 1);
            }
            if (x + 1 <= 7 && y + 2 <= 7)
            {
                if (board[x + 1][y + 2] == bN)
                    return 8 * (x + 1) + (y + 2);
            }
            if (x + 1 <= 7 && y - 2 >= 0)
            {
                if (board[x + 1][y - 2] == bN)
                    return 8 * (x + 1) + (y - 2);
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
                    if (board[x - i][y - i] == bB)
                    {
                        return 8 * (x - i) + (y - i);
                    }
                    else if (board[x - i][y - i] == bQ)
                    {
                        attackers_index[0] = 8 * (x - i) + (y - i);
                    }
                    else if (i == 1 && board[x - i][y - i] == bK)
                    {
                        attackers_index[1] = 8 * (x - i) + (y - i);
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
                    if (board[x - i][y + i] == bB)
                    {
                        return 8 * (x - i) + (y + i);
                    }
                    else if (board[x - i][y + i] == bQ)
                    {
                        attackers_index[0] = 8 * (x - i) + (y + i);
                    }
                    else if (i == 1 && board[x - i][y + i] == bK)
                    {
                        attackers_index[1] = 8 * (x - i) + (y + i);
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
                    if (board[x + i][y - i] == bB)
                    {
                        return 8 * (x + i) + (y - i);
                    }
                    else if (board[x + i][y - i] == bQ)
                    {
                        attackers_index[0] = 8 * (x + i) + (y - i);
                    }
                    else if (i == 1 && board[x + i][y - i] == bK)
                    {
                        attackers_index[1] = 8 * (x + i) + (y - i);
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
                    if (board[x + i][y + i] == bB)
                    {
                        return 8 * (x + i) + (y + i);
                    }
                    else if (board[x + i][y + i] == bQ)
                    {
                        attackers_index[0] = 8 * (x + i) + (y + i);
                    }
                    else if (i == 1 && board[x + i][y + i] == bK)
                    {
                        attackers_index[1] = 8 * (x + i) + (y + i);
                    }
                    break;
                }
            }

            // up
            for (int i = x - 1; i >= 0; i--)
            {
                if (board[i][y] != __)
                {
                    if (board[i][y] == bR)
                    {
                        return 8 * i + y;
                    }
                    else if (board[i][y] == bQ)
                    {
                        attackers_index[0] = 8 * i + y;
                    }
                    else if (i == x - 1 && board[i][y] == bK)
                    {
                        attackers_index[1] = 8 * i + y;
                    }
                    break;
                }
            }

            // left
            for (int i = y - 1; i >= 0; i--)
            {
                if (board[x][i] != __)
                {
                    if (board[x][i] == bR)
                    {
                        return 8 * x + i;
                    }
                    else if (board[x][i] == bQ)
                    {
                        attackers_index[0] = 8 * x + i;
                    }
                    else if (i == y - 1 && board[x][i] == bK)
                    {
                        attackers_index[1] = 8 * x + i;
                    }

                    break;
                }
            }

            // right
            for (int i = y + 1; i <= 7; i++)
            {
                if (board[x][i] != __)
                {
                    if (board[x][i] == bR)
                    {
                        return 8 * x + i;
                    }
                    else if (board[x][i] == bQ)
                    {
                        attackers_index[0] = 8 * x + i;
                    }
                    else if (i == y + 1 && board[x][i] == bK)
                    {
                        attackers_index[1] = 8 * x + i;
                    }
                    break;
                }
            }
            
            // down
            for (int i = x + 1; i <= 7; i++)
            {
                if (board[i][y] != __)
                {
                    if (board[i][y] == bR)
                    {
                        return 8 * i + y;
                    }
                    else if (board[i][y] == bQ)
                    {
                        attackers_index[0] = 8 * i + y;
                    }
                    else if (i == x + 1 && board[i][y] == bK)
                    {
                        attackers_index[1] = 8 * i + y;
                    }
                    break;
                }
            }
            break;
        }
    }

    if (attackers_index[0] != -2)
    {
        return attackers_index[0];
    }
    else if (attackers_index[1] != -2)
    {
        return attackers_index[1];
    }

    return -1;
}

// static exchange evaluation for quiescence search
inline int SEE(unsigned char board[8][8], int new_x, int new_y, int target, int color)
{
    int attacker_index = get_smallest_attacker(board, new_x, new_y, color);
    // exit if no attackers found
    if (attacker_index == -1)
        return target;
    unsigned char board_copy[8][8];
    int x, y;
    unsigned char piece;
    bool king_attack = false;
    int gain[32];
    int d = 0;
    gain[d] = target;
    memcpy(board_copy, board, sizeof(board_copy));

    do
    {
        d++;
        // convert indices to 2D
        x = attacker_index / 8;
        y = attacker_index % 8;

        if (board_copy[x][y] == wK || board_copy[x][y] == bK)
            king_attack = true;

        piece = board_copy[new_x][new_y];
        // make the capture
        makeMove_SEE(board_copy, x, y, new_x, new_y);
        color = -color;
        attacker_index = get_smallest_attacker(board_copy, new_x, new_y, color);

        // stop when king is captured
        if (king_attack && attacker_index != -1)
        {
            gain[d] = -gain[d - 1];
            break;
        }

        gain[d] = piece_value[piece] - gain[d - 1];

        // pruning that won't change the sign of the result
        if (((-gain[d - 1] >= gain[d]) ? -gain[d - 1] : gain[d]) < 0)
        {
            gain[d] = -gain[d - 1];
            break;
        }

    } while (attacker_index != -1);

    while (--d)
    {
        gain[d] = -((-gain[d] >= gain[d + 1]) ? -gain[d] : gain[d + 1]);
    }
    gain[0] = -((-gain[0] >= gain[1]) ? -gain[0] : gain[1]);

    return gain[0];
}

// static exchange evaluation for move ordering
inline int SEE_MO(unsigned char board[8][8], int att_x, int att_y, int new_x, int new_y, int target, int color)
{
    unsigned char board_copy[8][8];
    int x = att_x;
    int y = att_y;
    int attacker_index;
    bool king_attack = false;
    int gain[32];
    int d = 0;
    gain[d] = target;
    memcpy(board_copy, board, sizeof(board_copy));

    do
    {
        d++;
        gain[d] = piece_value[board_copy[x][y]] - gain[d - 1];

        if (board_copy[x][y] == wK || board_copy[x][y] == bK)
            king_attack = true;

        // pruning
        if (((-gain[d - 1] >= gain[d]) ? -gain[d - 1] : gain[d]) < 0)
        {
            if (!king_attack)
                break;
        }

        // make the capture
        makeMove_SEE(board_copy, x, y, new_x, new_y);
        attacker_index = get_smallest_attacker(board_copy, new_x, new_y, color);
        color = -color;

        // stop when king is captured
        if (king_attack && attacker_index != -1)
        {
            d--;
            if (d == 0)
                return -20000;
            break;
        }

        // convert indices to 2D
        x = attacker_index / 8;
        y = attacker_index % 8;

    } while (attacker_index != -1);

    while (--d)
    {
        gain[d - 1] = -((-gain[d - 1] >= gain[d]) ? -gain[d - 1] : gain[d]);
    }

    return gain[0];
}