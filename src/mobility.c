#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>
#include "mobility.h"
#include "movegen.h"

int wattack_count;
int battack_count;
int wattack_weight;
int battack_weight;

// check if this square is defended by pawns
static inline bool defended_by_wpawn(unsigned char board[8][8], int x, int y)
{
    if (x < 6)
    {
        switch (y)
        {
            case 0:
                if (board[x + 1][y + 1] == wP)
                {
                    return true;
                }
                break;
            case 7:
                if (board[x + 1][y - 1] == wP)
                {
                    return true;
                }
                break;
            default:
                if (board[x + 1][y - 1] == wP || board[x + 1][y + 1] == wP)
                {
                    return true;
                }
                break;
        }
    }

    return false;
}

static inline bool defended_by_bpawn(unsigned char board[8][8], int x, int y)
{
    if (x > 1)
    {
        switch (y)
        {
            case 0:
                if (board[x - 1][y + 1] == bP)
                {
                    return true;
                }
                break;
            case 7:
                if (board[x - 1][y - 1] == bP)
                {
                    return true;
                }
                break;
            default:
                if (board[x - 1][y - 1] == bP || board[x - 1][y + 1] == bP)
                {
                    return true;
                }
                break;
        }
    }

    return false;
}

// get king zone
// including king ring, king's position, and 3 forward positions
static inline bool wking_zone(int wkingx, int wkingy, int x, int y)
{
    if (y >= wkingy - 1 && y <= wkingy + 1 && x >= wkingx - 2 && x <= wkingx + 1)
    {
        return true;
    }

    return false;
}

static inline bool bking_zone(int bkingx, int bkingy, int x, int y)
{
    if (y >= bkingy - 1 && y <= bkingy + 1 && x >= bkingx - 1 && x <= bkingx + 2)
    {
        return true;
    }

    return false;
}

// mobility
// exclude space protected by enemy pawns for minor pieces
// exclude own queen for minor pieces
// exclude own blocked pawns and own pawns on rank 2 and 3
// exclude own king
// also consider attack on enemy king
inline int wknight_mobility(unsigned char board[8][8], int index_x, int index_y, int bkingx, int bkingy)
{
    int attack = 0;
    int move_count = 0;
    int x, y;

    for (int j = 0; j < 8; j++)
    {
        x = index_x + knight_moves_x[j];
        y = index_y + knight_moves_y[j];
        if (x & 8 || y & 8) // out of board
        {
            continue;
        }
        if (board[x][y] != wK && board[x][y] != wQ && (board[x][y] != wP || (x < 5 && board[x - 1][y] == __)))
        {
            if (!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if (bking_zone(bkingx, bkingy, x, y))
        {
            attack++;
        }
    }

    if (attack)
    {
        wattack_count++;
        wattack_weight += 2 * attack;
    }

    return 4 * (move_count - 4);
}

inline int bknight_mobility(unsigned char board[8][8], int index_x, int index_y, int wkingx, int wkingy)
{
    int attack = 0;
    int move_count = 0;
    int x, y;

    for (int j = 0; j < 8; j++)
    {
        x = index_x + knight_moves_x[j];
        y = index_y + knight_moves_y[j];
        if (x & 8 || y & 8) // out of board
        {
            continue;
        }
        if (board[x][y] != bK && board[x][y] != bQ && (board[x][y] != bP || (x > 2 && board[x + 1][y] == __)))
        {
            if (!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if (wking_zone(wkingx, wkingy, x, y))
        {
            attack++;
        }
    }

    if (attack)
    {
        battack_count++;
        battack_weight += 2 * attack;
    }

    return 4 * (move_count - 4);
}

inline int wbishop_mobility(unsigned char board[8][8], int index_x, int index_y, int bkingx, int bkingy)
{
    int attack = 0;
    int move_count = 0;
    int x, y;

    // up left
    for (int j = 1; j <= 7; j++)
    {
        x = index_x - j;
        y = index_y - j;
        if (x < 0 || y < 0)
        {
            break;
        }
        if (board[x][y] != wK && board[x][y] != wQ && (board[x][y] != wP || (x < 5 && board[x - 1][y] == __)))
        {
            if (!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if (bking_zone(bkingx, bkingy, x, y))
        {
            attack++;
        }
        if (board[x][y] != __)
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
        if (board[x][y] != wK && board[x][y] != wQ && (board[x][y] != wP || (x < 5 && board[x - 1][y] == __)))
        {
            if (!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if (bking_zone(bkingx, bkingy, x, y))
        {
            attack++;
        }
        if (board[x][y] != __)
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
        if (board[x][y] != wK && board[x][y] != wQ && (board[x][y] != wP || (x < 5 && board[x - 1][y] == __)))
        {
            if (!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if (bking_zone(bkingx, bkingy, x, y))
        {
            attack++;
        }
        if (board[x][y] != __)
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
        if (board[x][y] != wK && board[x][y] != wQ && (board[x][y] != wP || (x < 5 && board[x - 1][y] == __)))
        {
            if (!defended_by_bpawn(board, x, y))
                move_count++;
        }
        if (bking_zone(bkingx, bkingy, x, y))
        {
            attack++;
        }
        if (board[x][y] != __)
        {
            break;
        }
    }

    if (attack)
    {
        wattack_count++;
        wattack_weight += 2 * attack;
    }

    return 3 * (move_count - 7);
}

inline int bbishop_mobility(unsigned char board[8][8], int index_x, int index_y, int wkingx, int wkingy)
{
    int attack = 0;
    int move_count = 0;
    int x, y;

    // up left
    for (int j = 1; j <= 7; j++)
    {
        x = index_x - j;
        y = index_y - j;
        if (x < 0 || y < 0)
        {
            break;
        }
        if (board[x][y] != bK && board[x][y] != bQ && (board[x][y] != bP || (x > 2 && board[x + 1][y] == __)))
        {
            if (!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if (wking_zone(wkingx, wkingy, x, y))
        {
            attack++;
        }
        if (board[x][y] != __)
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
        if (board[x][y] != bK && board[x][y] != bQ && (board[x][y] != bP || (x > 2 && board[x + 1][y] == __)))
        {
            if (!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if (wking_zone(wkingx, wkingy, x, y))
        {
            attack++;
        }
        if (board[x][y] != __)
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
        if (board[x][y] != bK && board[x][y] != bQ && (board[x][y] != bP || (x > 2 && board[x + 1][y] == __)))
        {
            if (!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if (wking_zone(wkingx, wkingy, x, y))
        {
            attack++;
        }
        if (board[x][y] != __)
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
        if (board[x][y] != bK && board[x][y] != bQ && (board[x][y] != bP || (x > 2 && board[x + 1][y] == __)))
        {
            if (!defended_by_wpawn(board, x, y))
                move_count++;
        }
        if (wking_zone(wkingx, wkingy, x, y))
        {
            attack++;
        }
        if (board[x][y] != __)
        {
            break;
        }
    }

    if (attack)
    {
        battack_count++;
        battack_weight += 2 * attack;
    }

    return 3 * (move_count - 7);
}

inline int wrook_mobility(unsigned char board[8][8], int index_x, int index_y, int bkingx, int bkingy)
{
    int attack = 0;
    int move_count = 0;
    int x, y;

    // up
    for (int j = index_x - 1; j >= 0; j--)
    {
        x = j;
        y = index_y;
        if (board[x][y] != wK && (board[x][y] != wP || (x < 5 && board[x - 1][y] == __)))
        {
            move_count++;
        }
        if (bking_zone(bkingx, bkingy, x, y))
        {
            attack++;
        }
        if (board[x][y] != __)
        {
            break;
        }
    }
    // left
    for (int j = index_y - 1; j >= 0; j--)
    {
        x = index_x;
        y = j;
        if (board[x][y] != wK && (board[x][y] != wP || (x < 5 && board[x - 1][y] == __)))
        {
            move_count++;
        }
        if (bking_zone(bkingx, bkingy, x, y))
        {
            attack++;
        }
        if (board[x][y] != __)
        {
            break;
        }
    }
    // right
    for (int j = index_y + 1; j <= 7; j++)
    {
        x = index_x;
        y = j;
        if (board[x][y] != wK && (board[x][y] != wP || (x < 5 && board[x - 1][y] == __)))
        {
            move_count++;
        }
        if (bking_zone(bkingx, bkingy, x, y))
        {
            attack++;
        }
        if (board[x][y] != __)
        {
            break;
        }
    }
    // down
    for (int j = index_x + 1; j <= 7; j++)
    {
        x = j;
        y = index_y;
        if (board[x][y] != wK && (board[x][y] != wP || (x < 5 && board[x - 1][y] == __)))
        {
            move_count++;
        }
        if (bking_zone(bkingx, bkingy, x, y))
        {
            attack++;
        }
        if (board[x][y] != __)
        {
            break;
        }
    }

    if (attack)
    {
        wattack_count++;
        wattack_weight += 3 * attack;
    }

    return 2 * (move_count - 7);
}

inline int brook_mobility(unsigned char board[8][8], int index_x, int index_y, int wkingx, int wkingy)
{
    int attack = 0;
    int move_count = 0;
    int x, y;

    // up
    for (int j = index_x - 1; j >= 0; j--)
    {
        x = j;
        y = index_y;
        if (board[x][y] != bK && (board[x][y] != bP || (x > 2 && board[x + 1][y] == __)))
        {
            move_count++;
        }
        if (wking_zone(wkingx, wkingy, x, y))
        {
            attack++;
        }
        if (board[x][y] != __)
        {
            break;
        }
    }
    // left
    for (int j = index_y - 1; j >= 0; j--)
    {
        x = index_x;
        y = j;
        if (board[x][y] != bK && (board[x][y] != bP || (x > 2 && board[x + 1][y] == __)))
        {
            move_count++;
        }
        if (wking_zone(wkingx, wkingy, x, y))
        {
            attack++;
        }
        if (board[x][y] != __)
        {
            break;
        }
    }
    // right
    for (int j = index_y + 1; j <= 7; j++)
    {
        x = index_x;
        y = j;
        if (board[x][y] != bK && (board[x][y] != bP || (x > 2 && board[x + 1][y] == __)))
        {
            move_count++;
        }
        if (wking_zone(wkingx, wkingy, x, y))
        {
            attack++;
        }
        if (board[x][y] != __)
        {
            break;
        }
    }
    // down
    for (int j = index_x + 1; j <= 7; j++)
    {
        x = j;
        y = index_y;
        if (board[x][y] != bK && (board[x][y] != bP || (x > 2 && board[x + 1][y] == __)))
        {
            move_count++;
        }
        if (wking_zone(wkingx, wkingy, x, y))
        {
            attack++;
        }
        if (board[x][y] != __)
        {
            break;
        }
    }

    if (attack)
    {
        battack_count++;
        battack_weight += 3 * attack;
    }

    return 2 * (move_count - 7);
}

inline int wqueen_mobility(unsigned char board[8][8], int index_x, int index_y, int bkingx, int bkingy)
{
    int attack = 0;
    int move_count = 0;
    int x, y;

    // up left
    for (int j = 1; j <= 7; j++)
    {
        x = index_x - j;
        y = index_y - j;
        if (x < 0 || y < 0)
        {
            break;
        }
        if (board[x][y] != wK && (board[x][y] != wP || (x < 5 && board[x - 1][y] == __)))
        {
            move_count++;
        }
        if (bking_zone(bkingx, bkingy, x, y))
        {
            attack++;
        }
        if (board[x][y] != __)
        {
            break;
        }
    }
    // up
    for (int j = index_x - 1; j >= 0; j--)
    {
        x = j;
        y = index_y;
        if (board[x][y] != wK && (board[x][y] != wP || (x < 5 && board[x - 1][y] == __)))
        {
            move_count++;
        }
        if (bking_zone(bkingx, bkingy, x, y))
        {
            attack++;
        }
        if (board[x][y] != __)
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
        if (board[x][y] != wK && (board[x][y] != wP || (x < 5 && board[x - 1][y] == __)))
        {
            move_count++;
        }
        if (bking_zone(bkingx, bkingy, x, y))
        {
            attack++;
        }
        if (board[x][y] != __)
        {
            break;
        }
    }
    // left
    for (int j = index_y - 1; j >= 0; j--)
    {
        x = index_x;
        y = j;
        if (board[x][y] != wK && (board[x][y] != wP || (x < 5 && board[x - 1][y] == __)))
        {
            move_count++;
        }
        if (bking_zone(bkingx, bkingy, x, y))
        {
            attack++;
        }
        if (board[x][y] != __)
        {
            break;
        }
    }
    // right
    for (int j = index_y + 1; j <= 7; j++)
    {
        x = index_x;
        y = j;
        if (board[x][y] != wK && (board[x][y] != wP || (x < 5 && board[x - 1][y] == __)))
        {
            move_count++;
        }
        if (bking_zone(bkingx, bkingy, x, y))
        {
            attack++;
        }
        if (board[x][y] != __)
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
        if (board[x][y] != wK && (board[x][y] != wP || (x < 5 && board[x - 1][y] == __)))
        {
            move_count++;
        }
        if (bking_zone(bkingx, bkingy, x, y))
        {
            attack++;
        }
        if (board[x][y] != __)
        {
            break;
        }
    }
    // down
    for (int j = index_x + 1; j <= 7; j++)
    {
        x = j;
        y = index_y;
        if (board[x][y] != wK && (board[x][y] != wP || (x < 5 && board[x - 1][y] == __)))
        {
            move_count++;
        }
        if (bking_zone(bkingx, bkingy, x, y))
        {
            attack++;
        }
        if (board[x][y] != __)
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
        if (board[x][y] != wK && (board[x][y] != wP || (x < 5 && board[x - 1][y] == __)))
        {
            move_count++;
        }
        if (bking_zone(bkingx, bkingy, x, y))
        {
            attack++;
        }
        if (board[x][y] != __)
        {
            break;
        }
    }

    if (attack)
    {
        wattack_count++;
        wattack_weight += 4 * attack;
    }

    return 1 * (move_count - 14);
}

inline int bqueen_mobility(unsigned char board[8][8], int index_x, int index_y, int wkingx, int wkingy)
{
    int attack = 0;
    int move_count = 0;
    int x, y;

    // up left
    for (int j = 1; j <= 7; j++)
    {
        x = index_x - j;
        y = index_y - j;
        if (x < 0 || y < 0)
        {
            break;
        }
        if (board[x][y] != bK && (board[x][y] != bP || (x > 2 && board[x + 1][y] == __)))
        {
            move_count++;
        }
        if (wking_zone(wkingx, wkingy, x, y))
        {
            attack++;
        }
        if (board[x][y] != __)
        {
            break;
        }
    }
    // up
    for (int j = index_x - 1; j >= 0; j--)
    {
        x = j;
        y = index_y;
        if (board[x][y] != bK && (board[x][y] != bP || (x > 2 && board[x + 1][y] == __)))
        {
            move_count++;
        }
        if (wking_zone(wkingx, wkingy, x, y))
        {
            attack++;
        }
        if (board[x][y] != __)
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
        if (board[x][y] != bK && (board[x][y] != bP || (x > 2 && board[x + 1][y] == __)))
        {
            move_count++;
        }
        if (wking_zone(wkingx, wkingy, x, y))
        {
            attack++;
        }
        if (board[x][y] != __)
        {
            break;
        }
    }
    // left
    for (int j = index_y - 1; j >= 0; j--)
    {
        x = index_x;
        y = j;
        if (board[x][y] != bK && (board[x][y] != bP || (x > 2 && board[x + 1][y] == __)))
        {
            move_count++;
        }
        if (wking_zone(wkingx, wkingy, x, y))
        {
            attack++;
        }
        if (board[x][y] != __)
        {
            break;
        }
    }
    // right
    for (int j = index_y + 1; j <= 7; j++)
    {
        x = index_x;
        y = j;
        if (board[x][y] != bK && (board[x][y] != bP || (x > 2 && board[x + 1][y] == __)))
        {
            move_count++;
        }
        if (wking_zone(wkingx, wkingy, x, y))
        {
            attack++;
        }
        if (board[x][y] != __)
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
        if (board[x][y] != bK && (board[x][y] != bP || (x > 2 && board[x + 1][y] == __)))
        {
            move_count++;
        }
        if (wking_zone(wkingx, wkingy, x, y))
        {
            attack++;
        }
        if (board[x][y] != __)
        {
            break;
        }
    }
    // down
    for (int j = index_x + 1; j <= 7; j++)
    {
        x = j;
        y = index_y;
        if (board[x][y] != bK && (board[x][y] != bP || (x > 2 && board[x + 1][y] == __)))
        {
            move_count++;
        }
        if (wking_zone(wkingx, wkingy, x, y))
        {
            attack++;
        }
        if (board[x][y] != __)
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
        if (board[x][y] != bK && (board[x][y] != bP || (x > 2 && board[x + 1][y] == __)))
        {
            move_count++;
        }
        if (wking_zone(wkingx, wkingy, x, y))
        {
            attack++;
        }
        if (board[x][y] != __)
        {
            break;
        }
    }
    
    if (attack)
    {
        battack_count++;
        battack_weight += 4 * attack;
    }

    return 1 * (move_count - 14);
}