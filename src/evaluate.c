#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>
#include "evaluate.h"
#include "board.h"
#include "mobility.h"
#include "evalhelpers.h"
#include "psqt.h"
#include "transposition.h"

// 1: black
// -1: white
int evaluate(BOARD *pos, unsigned char board[8][8], int color)
{
    // eval tt probe
    struct Eval *entry = probeEvalTT(pos->key);
    if (entry != NULL)
    {
        return entry->evaluation;
    }

    int P_count = pos->piece_count[wP], R_count = pos->piece_count[wR], N_count = pos->piece_count[wN];
    int B_count = pos->piece_count[wB], Q_count = pos->piece_count[wQ]; // white chess piece counts
    int p_count = pos->piece_count[bP], r_count = pos->piece_count[bR], n_count = pos->piece_count[bN];
    int b_count = pos->piece_count[bB], q_count = pos->piece_count[bQ]; // black chess piece counts
    int white_king_x = pos->piece_list[wK][0] / 8; // get white king's coordinate
    int white_king_y = pos->piece_list[wK][0] % 8;
    int black_king_x = pos->piece_list[bK][0] / 8; // get black king's coordinate
    int black_king_y = pos->piece_list[bK][0] % 8;
    int midgame_white = 0, midgame_black = 0, endgame_white = 0, endgame_black = 0;
    int points = 0, other_bonus_white = 0, other_bonus_black = 0;
    int wpawn_mg = 0, bpawn_mg = 0, pawn_mg = 0;
    int wpawn_eg = 0, bpawn_eg = 0, pawn_eg = 0;
    int tempo = 0;
    int wqueen_mob, bqueen_mob, wrook_mob, brook_mob;
    int wqueen_tropism, bqueen_tropism, wrook_tropism, brook_tropism;
    int wknight_tropism, bknight_tropism, wbishop_tropism, bbishop_tropism;
    int x, y;
    bool passed;
    wattack_count = battack_count = wattack_weight = battack_weight = 0;

    // pawn tt probe
    struct Pawn *pEntry = probePawnTT(pos->pawn_key);
    if (pEntry != NULL)
    {
        pawn_mg = pEntry->eval_mg;
        pawn_eg = pEntry->eval_eg;
    }
    else
    {
        // white pawn
        for (int i = 0; i < P_count; i++)
        {
            x = pos->piece_list[wP][i] / 8;
            y = pos->piece_list[wP][i] % 8;

            passed = false;
            if (candidate_passed_white(board, x, y))
            {
                passed = true;
                wpawn_mg += passedpawn_white[x];
                wpawn_eg += passedpawn_white[x];
                if (phalanx_white(board, x, y))
                {
                    wpawn_mg += PHALANX;
                    wpawn_eg += PHALANX;
                }
                // king proximity
                wpawn_eg -= (abs(x - white_king_x) + abs(y - white_king_y) - 7) * 3;
                wpawn_eg += (abs(x - black_king_x) + abs(y - black_king_y) - 7) * 3;
            }
            else
            {
                wpawn_mg += white_pawn_midgame[x][y];
                wpawn_eg += white_pawn_endgame[x][y];
            }

            // connected pawn bonus
            if (connected_white(board, x, y))
            {
                if (passed)
                {
                    wpawn_mg += CONNECTEDPAWN * 2;
                    wpawn_eg += CONNECTEDPAWN * 2;
                }
                else
                {
                    wpawn_mg += CONNECTEDPAWN;
                    wpawn_eg += CONNECTEDPAWN;
                }
            }
            // doubled pawn penalty 
            else if (board[x + 1][y] == wP)
            {
                wpawn_mg -= DOUBLEDPAWNMG;
                wpawn_eg -= DOUBLEDPAWNEG;
            }

            // isolated / backward pawn
            if (isolated_white(board, y))
            {
                wpawn_mg -= ISOLATEDPAWNMG;
                wpawn_eg -= ISOLATEDPAWNEG;
            }
            else if (backward_white(board, x, y))
            {
                wpawn_mg -= BACKWARDPAWNMG;
                wpawn_eg -= BACKWARDPAWNEG;
            }
        }

        // black pawn
        for (int i = 0; i < p_count; i++)
        {
            x = pos->piece_list[bP][i] / 8;
            y = pos->piece_list[bP][i] % 8;

            passed = false;
            if (candidate_passed_black(board, x, y))
            {
                passed = true;
                bpawn_mg += passedpawn_black[x];
                bpawn_eg += passedpawn_black[x];
                if (phalanx_black(board, x, y))
                {
                    bpawn_mg += PHALANX;
                    bpawn_eg += PHALANX;
                }
                // king proximity
                bpawn_eg -= (abs(x - black_king_x) + abs(y - black_king_y) - 7) * 3;
                bpawn_eg += (abs(x - white_king_x) + abs(y - white_king_y) - 7) * 3;
            }
            else
            {
                bpawn_mg += black_pawn_midgame[x][y];
                bpawn_eg += black_pawn_endgame[x][y];
            }

            // connected pawn bonus
            if (connected_black(board, x, y))
            {
                if (passed)
                {
                    bpawn_mg += CONNECTEDPAWN * 2;
                    bpawn_eg += CONNECTEDPAWN * 2;
                }
                else
                {
                    bpawn_mg += CONNECTEDPAWN;
                    bpawn_eg += CONNECTEDPAWN;
                }
            }
            // doubled pawn penalty
            else if (board[x - 1][y] == bP)
            {
                bpawn_mg -= DOUBLEDPAWNMG;
                bpawn_eg -= DOUBLEDPAWNEG;
            }

            // isolated / backward pawn
            if (isolated_black(board, y))
            {
                bpawn_mg -= ISOLATEDPAWNMG;
                bpawn_eg -= ISOLATEDPAWNEG;
            }
            else if (backward_black(board, x, y))
            {
                bpawn_mg -= BACKWARDPAWNMG;
                bpawn_eg -= BACKWARDPAWNEG;
            }
        }

        // pawn shield bonus and pawn storm penalty
        if (white_king_x > 5 && white_king_y > 4)
        {
            if (board[6][5] == wP)
                wpawn_mg += 15;
            else if (board[5][5] == wP)
                wpawn_mg += 10;

            if (board[5][5] == bP)
                wpawn_mg -= 10;
            else if (board[4][5] == bP)
                wpawn_mg -= 5;

            if (board[6][6] == wP)
                wpawn_mg += 15;
            else if (board[5][6] == wP)
                wpawn_mg += 10;

            if (board[5][6] == bP)
                wpawn_mg -= 10;
            else if (board[4][6] == bP)
                wpawn_mg -= 5;

            if (board[6][7] == wP)
                wpawn_mg += 15;
            else if (board[5][7] == wP)
                wpawn_mg += 10;

            if (board[5][7] == bP)
                wpawn_mg -= 10;
            else if (board[4][7] == bP)
                wpawn_mg -= 5;
        }
        else if (white_king_x > 5 && white_king_y < 3)
        {
            if (board[6][0] == wP)
                wpawn_mg += 15;
            else if (board[5][0] == wP)
                wpawn_mg += 10;

            if (board[5][0] == bP)
                wpawn_mg -= 10;
            else if (board[4][0] == bP)
                wpawn_mg -= 5;

            if (board[6][1] == wP)
                wpawn_mg += 15;
            else if (board[5][1] == wP)
                wpawn_mg += 10;

            if (board[5][1] == bP)
                wpawn_mg -= 10;
            else if (board[4][1] == bP)
                wpawn_mg -= 5;

            if (board[6][2] == wP)
                wpawn_mg += 15;
            else if (board[5][2] == wP)
                wpawn_mg += 10;

            if (board[5][2] == bP)
                wpawn_mg -= 10;
            else if (board[4][2] == bP)
                wpawn_mg -= 5;
        }

        if (black_king_x < 2 && black_king_y > 4)
        {
            if (board[1][5] == bP)
                bpawn_mg += 15;
            else if (board[2][5] == bP)
                bpawn_mg += 10;

            if (board[2][5] == wP)
                bpawn_mg -= 10;
            else if (board[3][5] == wP)
                bpawn_mg -= 5;

            if (board[1][6] == bP)
                bpawn_mg += 15;
            else if (board[2][6] == bP)
                bpawn_mg += 10;

            if (board[2][6] == wP)
                bpawn_mg -= 10;
            else if (board[3][6] == wP)
                bpawn_mg -= 5;

            if (board[1][7] == bP)
                bpawn_mg += 15;
            else if (board[2][7] == bP)
                bpawn_mg += 10;

            if (board[2][7] == wP)
                bpawn_mg -= 10;
            else if (board[3][7] == wP)
                bpawn_mg -= 5;
        }
        else if (black_king_x < 2 && black_king_y < 3)
        {
            if (board[1][0] == bP)
                bpawn_mg += 15;
            else if (board[2][0] == bP)
                bpawn_mg += 10;

            if (board[2][0] == wP)
                bpawn_mg -= 10;
            else if (board[3][0] == wP)
                bpawn_mg -= 5;

            if (board[1][1] == bP)
                bpawn_mg += 15;
            else if (board[2][1] == bP)
                bpawn_mg += 10;

            if (board[2][1] == wP)
                bpawn_mg -= 10;
            else if (board[3][1] == wP)
                bpawn_mg -= 5;

            if (board[1][2] == bP)
                bpawn_mg += 15;
            else if (board[2][2] == bP)
                bpawn_mg += 10;

            if (board[2][2] == wP)
                bpawn_mg -= 10;
            else if (board[3][2] == wP)
                bpawn_mg -= 5;
        }

        // open files next to king penalty
        wpawn_mg -= wking_file(board, white_king_y);
        bpawn_mg -= bking_file(board, black_king_y);

        // for king position bonus midgame
        wpawn_mg += white_king_midgame[white_king_x][white_king_y];
        bpawn_mg += black_king_midgame[black_king_x][black_king_y];
        // king position bonus endgame
        wpawn_eg += white_king_endgame[white_king_x][white_king_y];
        bpawn_eg += black_king_endgame[black_king_x][black_king_y];

        pawn_mg = bpawn_mg - wpawn_mg;
        pawn_eg = bpawn_eg - wpawn_eg;

        // pawn tt store
        storePawnTT(pos->pawn_key, pawn_mg, pawn_eg);
    }

    // white rook
    for (int i = 0; i < R_count; i++)
    {
        x = pos->piece_list[wR][i] / 8;
        y = pos->piece_list[wR][i] % 8;

        other_bonus_white += white_rook[x][y];

        if (openFile(board, y))
        {
            midgame_white += ROOKOPENFILEMG;
            endgame_white += ROOKOPENFILEEG;
        }
        else if (semiOpenFile_white(board, y))
        {
            midgame_white += ROOKHALFFILEMG;
            endgame_white += ROOKHALFFILEEG;
        }

        // bonus for being on the same file as any queen
        if (queenFile(pos, y))
        {
            other_bonus_white += ROOKQUEENFILE;
        }

        // mobility
        wrook_mob = wrook_mobility(board, x, y, black_king_x, black_king_y);
        midgame_white += wrook_mob;
        endgame_white += wrook_mob * 2;
        // king tropism
        wrook_tropism = abs(x - black_king_x) + abs(y - black_king_y) - 7;
        midgame_black += wrook_tropism * 2;
        endgame_black += wrook_tropism * 1;

        // trapped rook penalty
        // when mobility < 4
        if (wrook_mob < -6)
        {
            switch (pos->piece_list[wK][0])
            {
                case f1:
                    if (x >= 6 && y >= 6)
                    {
                        midgame_white -= 40;
                    }
                    break;
                case g1:
                    if (x >= 6 && y >= 6)
                    {
                        midgame_white -= 40;
                    }
                    break;
                case b1:
                    if (x >= 6 && y <= 1)
                    {
                        midgame_white -= 40;
                    }
                    break;
                case c1:
                    if (x >= 6 && y <= 1)
                    {
                        midgame_white -= 40;
                    }
                    break;
            }
        }
    }

    // white knight
    for (int i = 0; i < N_count; i++)
    {
        x = pos->piece_list[wN][i] / 8;
        y = pos->piece_list[wN][i] % 8;

        other_bonus_white += white_knight[x][y];

        if (outpost_white(board, x, y))
        {
            other_bonus_white += OUTPOST; // outpost bonus
        }

        // mobility
        other_bonus_white += wknight_mobility(board, x, y, black_king_x, black_king_y);
        // king tropism
        wknight_tropism = abs(x - black_king_x) + abs(y - black_king_y) - 7;
        midgame_black += wknight_tropism * 3;
        endgame_black += wknight_tropism * 3;

        // trapped pieces penalty
        switch (pos->piece_list[wN][i])
        {
            case a8:
                if (board[1][0] == bP || board[1][2] == bP)
                {
                    other_bonus_white -= 100;
                }
                break;
            case h8:
                if (board[1][7] == bP || board[1][5] == bP)
                {
                    other_bonus_white -= 100;
                }
                break;
            case a7:
                if (board[2][0] == bP && board[1][1] == bP)
                {
                    other_bonus_white -= 100;
                }
                break;
            case h7:
                if (board[2][7] == bP && board[1][6] == bP)
                {
                    other_bonus_white -= 100;
                }
                break;
            case c3:
                if (board[6][2] == wP && board[4][3] == wP && board[4][4] != wP)
                {
                    other_bonus_white -= 5; // c3 knight penalty
                }
                break;
        }
    }

    // white bishop
    for (int i = 0; i < B_count; i++)
    {
        x = pos->piece_list[wB][i] / 8;
        y = pos->piece_list[wB][i] % 8;

        other_bonus_white += white_bishop[x][y];

        if (badBishop_white(board, x, y))
        {
            other_bonus_white -= BADBISHOP; // bad bishop penalty
        }
        if (outpost_white(board, x, y))
        {
            other_bonus_white += OUTPOST; // outpost bonus
        }

        // mobility
        other_bonus_white += wbishop_mobility(board, x, y, black_king_x, black_king_y);
        // king tropism
        wbishop_tropism = abs(x - black_king_x) + abs(y - black_king_y) - 7;
        midgame_black += wbishop_tropism * 2;
        endgame_black += wbishop_tropism * 1;

        // trapped pieces penalty
        switch (pos->piece_list[wB][i])
        {
            case a7:
                if (board[2][1] == bP && board[1][2] == bP)
                {
                    other_bonus_white -= 100;
                }
                break;
            case h7:
                if (board[2][6] == bP && board[1][5] == bP)
                {
                    other_bonus_white -= 100;
                }
                break;
            case a6:
                if (board[3][1] == bP && board[2][2] == bP)
                {
                    other_bonus_white -= 50;
                }
                break;
            case h6:
                if (board[3][6] == bP && board[2][5] == bP)
                {
                    other_bonus_white -= 50;
                }
                break;
            case f1:
                if (board[7][6] == wK)
                {
                    other_bonus_white += RETURNINGBISHOP;
                }
                if (board[6][4] == wP && board[5][4] != __)
                {
                    other_bonus_white -= 20; // bishop no development penalty
                }
                break;
            case c1:
                if (board[7][1] == wK)
                {
                    other_bonus_white += RETURNINGBISHOP;
                }
                if (board[6][3] == wP && board[5][3] != __)
                {
                    other_bonus_white -= 20; // bishop no development penalty
                }
                break;
        }
    }

    // white queen
    for (int i = 0; i < Q_count; i++)
    {
        x = pos->piece_list[wQ][i] / 8;
        y = pos->piece_list[wQ][i] % 8;

        other_bonus_white += white_queen[x][y];

        // queen early development penalty
        if (x < 6)
        {
            if (board[7][1] == wN)
            {
                other_bonus_white -= 4;
            }
            if (board[7][2] == wB)
            {
                other_bonus_white -= 4;
            }
            if (board[7][5] == wB)
            {
                other_bonus_white -= 4;
            }
            if (board[7][6] == wN)
            {
                other_bonus_white -= 4;
            }
        }

        // mobility
        wqueen_mob = wqueen_mobility(board, x, y, black_king_x, black_king_y);
        midgame_white += wqueen_mob;
        endgame_white += wqueen_mob * 2;
        // king tropism
        wqueen_tropism = abs(x - black_king_x) + abs(y - black_king_y) - 7;
        midgame_black += wqueen_tropism * 2;
        endgame_black += wqueen_tropism * 4;
    }

    // black rook
    for (int i = 0; i < r_count; i++)
    {
        x = pos->piece_list[bR][i] / 8;
        y = pos->piece_list[bR][i] % 8;

        other_bonus_black += black_rook[x][y];

        if (openFile(board, y))
        {
            midgame_black += ROOKOPENFILEMG;
            endgame_black += ROOKOPENFILEEG;
        }
        else if (semiOpenFile_black(board, y))
        {
            midgame_black += ROOKHALFFILEMG;
            endgame_black += ROOKHALFFILEEG;
        }

        // bonus for being on the same file as any queen
        if (queenFile(pos, y))
        {
            other_bonus_black += ROOKQUEENFILE;
        }

        // mobility
        brook_mob = brook_mobility(board, x, y, white_king_x, white_king_y);
        midgame_black += brook_mob;
        endgame_black += brook_mob * 2;
        // king tropism
        brook_tropism = abs(x - white_king_x) + abs(y - white_king_y) - 7;
        midgame_white += brook_tropism * 2;
        endgame_white += brook_tropism * 1;

        // trapped rook penalty
        // when mobility < 4
        if (brook_mob < -6)
        {
            switch (pos->piece_list[bK][0])
            {
                case f8:
                    if (x <= 1 && y >= 6)
                    {
                        midgame_black -= 40;
                    }
                    break;
                case g8:
                    if (x <= 1 && y >= 6)
                    {
                        midgame_black -= 40;
                    }
                    break;
                case b8:
                    if (x <= 1 && y <= 1)
                    {
                        midgame_black -= 40;
                    }
                    break;
                case c8:
                    if (x <= 1 && y <= 1)
                    {
                        midgame_black -= 40;
                    }
                    break;
            }
        }
    }

    // black knight
    for (int i = 0; i < n_count; i++)
    {
        x = pos->piece_list[bN][i] / 8;
        y = pos->piece_list[bN][i] % 8;

        other_bonus_black += black_knight[x][y];

        if (outpost_black(board, x, y))
        {
            other_bonus_black += OUTPOST; // outpost bonus
        }

        // mobility
        other_bonus_black += bknight_mobility(board, x, y, white_king_x, white_king_y);
        // king tropism
        bknight_tropism = abs(x - white_king_x) + abs(y - white_king_y) - 7;
        midgame_white += bknight_tropism * 3;
        endgame_white += bknight_tropism * 3;

        // trapped pieces penalty
        switch (pos->piece_list[bN][i])
        {
            case a1:
                if (board[6][0] == wP || board[6][2] == wP)
                {
                    other_bonus_black -= 100;
                }
                break;
            case h1:
                if (board[6][7] == wP || board[6][5] == wP)
                {
                    other_bonus_black -= 100;
                }
                break;
            case a2:
                if (board[5][0] == wP && board[6][1] == wP)
                {
                    other_bonus_black -= 100;
                }
                break;
            case h2:
                if (board[5][7] == wP && board[6][6] == wP)
                {
                    other_bonus_black -= 100;
                }
                break;
            case c6:
                if (board[1][2] == bP && board[3][3] == bP && board[3][4] != bP)
                {
                    other_bonus_black -= 5; // c6 knight penalty
                }
                break;
        }
    }

    // black bishop
    for (int i = 0; i < b_count; i++)
    {
        x = pos->piece_list[bB][i] / 8;
        y = pos->piece_list[bB][i] % 8;

        other_bonus_black += black_bishop[x][y];

        if (badBishop_black(board, x, y))
        {
            other_bonus_black -= BADBISHOP; // bad bishop penalty
        }
        if (outpost_black(board, x, y))
        {
            other_bonus_black += OUTPOST; // outpost bonus
        }

        // mobility
        other_bonus_black += bbishop_mobility(board, x, y, white_king_x, white_king_y);
        // king tropism
        bbishop_tropism = abs(x - white_king_x) + abs(y - white_king_y) - 7;
        midgame_white += bbishop_tropism * 2;
        endgame_white += bbishop_tropism * 1;

        // trapped pieces penalty
        switch (pos->piece_list[bB][i])
        {
            case a2:
                if (board[5][1] == wP && board[6][2] == wP)
                {
                    other_bonus_black -= 100;
                }
                break;
            case h2:
                if (board[5][6] == wP && board[6][5] == wP)
                {
                    other_bonus_black -= 100;
                }
                break;
            case a3:
                if (board[4][1] == wP && board[5][2] == wP)
                {
                    other_bonus_black -= 50;
                }
                break;
            case h3:
                if (board[4][6] == wP && board[5][5] == wP)
                {
                    other_bonus_black -= 50;
                }
                break;
            case f8:
                if (board[0][6] == bK)
                {
                    other_bonus_black += RETURNINGBISHOP;
                }
                if (board[1][4] == bP && board[2][4] != __)
                {
                    other_bonus_black -= 20; // bishop no development penalty
                }
                break;
            case c8:
                if (board[0][1] == bK)
                {
                    other_bonus_black += RETURNINGBISHOP;
                }
                if (board[1][3] == bP && board[2][3] != __)
                {
                    other_bonus_black -= 20; // bishop no development penalty
                }
                break;
        }
    }

    // black queen
    for (int i = 0; i < q_count; i++)
    {
        x = pos->piece_list[bQ][i] / 8;
        y = pos->piece_list[bQ][i] % 8;

        other_bonus_black += black_queen[x][y];

        // queen early development penalty
        if (x > 1)
        {
            if (board[0][1] == bN)
            {
                other_bonus_black -= 4;
            }
            if (board[0][2] == bB)
            {
                other_bonus_black -= 4;
            }
            if (board[0][5] == bB)
            {
                other_bonus_black -= 4;
            }
            if (board[0][6] == bN)
            {
                other_bonus_black -= 4;
            }
        }

        // mobility
        bqueen_mob = bqueen_mobility(board, x, y, white_king_x, white_king_y);
        midgame_black += bqueen_mob;
        endgame_black += bqueen_mob * 2;
        // king tropism
        bqueen_tropism = abs(x - white_king_x) + abs(y - white_king_y) - 7;
        midgame_white += bqueen_tropism * 2;
        endgame_white += bqueen_tropism * 4;
    }

    // game phase based on non-pawn materials
    int phase = N_count + n_count + B_count + b_count + R_count * 2 + r_count * 2 + Q_count * 4 + q_count * 4;
    // tempo
    if (phase > 4)
        tempo = color * TEMPO;

    // sum up scores from black's perspective
    points = p_count * 100 + r_count * (500 + rook_val[p_count]) + n_count * (325 + knight_val[p_count]) + b_count * 335 + q_count * 975
        + ((b_count >= 2) ? 1 : 0) * BISHOPPAIR - ((n_count >= 2) ? 1 : 0) * KNIGHTPAIR - ((r_count >= 2) ? 1 : 0) * ROOKPAIR + other_bonus_black
        - P_count * 100 - R_count * (500 + rook_val[P_count]) - N_count * (325 + knight_val[P_count]) - B_count * 335 - Q_count * 975
        - ((B_count >= 2) ? 1 : 0) * BISHOPPAIR + ((N_count >= 2) ? 1 : 0) * KNIGHTPAIR + ((R_count >= 2) ? 1 : 0) * ROOKPAIR - other_bonus_white
        + tempo;

    // adjust phase score based on materials
    if (phase > 24)
        phase = 24;
    int mg_weight = phase;
    int eg_weight = 24 - mg_weight;
    points += (((midgame_black - midgame_white + pawn_mg) * mg_weight + (endgame_black - endgame_white + pawn_eg) * eg_weight) / 24);

    // king attack score
    // disabled when the number of attackers is less than 2 or there is no queen
    if (wattack_count < 2 || !Q_count)
        wattack_weight = 0;
    if (battack_count < 2 || !q_count)
        battack_weight = 0;
    points += safety_table[battack_weight];
    points -= safety_table[wattack_weight];

    // material draw
    // score as 0 for insufficient bishops and knights
    // score half of the points for insufficient rooks
    if (!P_count && !p_count)
    {
        if (!R_count && !r_count && !Q_count && !q_count)
        {
            if (!B_count && !b_count)
            {
                if (N_count < 3 && n_count < 3)
                {
                    // tt store
                    storeEvalTT(pos->key, 0);
                    return 0;
                }
            }
            else if (!N_count && !n_count)
            {
                if (abs(B_count - b_count) < 2)
                {
                    // tt store
                    storeEvalTT(pos->key, 0);
                    return 0;
                }
            }
            else if ((N_count < 3 && !B_count) || (B_count == 1 && !N_count))
            {
                if ((n_count < 3 && !b_count) || (b_count == 1 && !n_count))
                {
                    // tt store
                    storeEvalTT(pos->key, 0);
                    return 0;
                }
            }
        }
        else if (!Q_count && !q_count)
        {
            if (R_count == 1 && r_count == 1)
            {
                if (((N_count + B_count) < 2 && (n_count + b_count) == 0) || ((N_count + B_count) == 0 && (n_count + b_count) < 2))
                {
                    // tt store
                    storeEvalTT(pos->key, color * points / 2);
                    return color * points / 2;
                }
            }
            else if (R_count == 1 && !r_count)
            {
                if ((N_count + B_count == 0) && (((n_count + b_count) == 1) || ((n_count + b_count) == 2)))
                {
                    // tt store
                    storeEvalTT(pos->key, color * points / 2);
                    return color * points / 2;
                }
            }
            else if (r_count == 1 && !R_count)
            {
                if ((n_count + b_count == 0) && (((N_count + B_count) == 1) || ((N_count + B_count) == 2)))
                {
                    // tt store
                    storeEvalTT(pos->key, color * points / 2);
                    return color * points / 2;
                }
            }
        }
    }

    // tt store
    storeEvalTT(pos->key, color * points);
    return color * points;
}