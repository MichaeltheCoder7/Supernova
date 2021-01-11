#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>
#include "syzygy.h"
#include "board.h"
#include "move.h"
#include "Fathom/tbprobe.h"

int TB_DEPTH;
uint64_t setMask[64];

// set masks for bitboard conversion
inline void init_setMask()
{
    for (int i = 0; i < 64; i++)
    {
        setMask[i] = 0ULL;
    }

    // reverse the ranks because of different representations
    for (int i = 0; i < 64; i++)
    {
        setMask[(56 - 16 * (i / 8)) + i] |= 1ULL << i;
    }
}

inline unsigned tablebasesProbeWDL(BOARD *pos, int depth, int color)
{
    // do not probe when # of pieces is greater than TB_LARGEST
    // or half move counter is not zero
    // or with castling rights
    if ((unsigned int) pos->piece_num > TB_LARGEST || pos->halfmove_counter
        || pos->ksw || pos->ksb || pos->qsw || pos->qsb)
        return TB_RESULT_FAILED;

    // do not probe when depth is less than probe depth
    if (depth < TB_DEPTH && (unsigned int) pos->piece_num == TB_LARGEST)
        return TB_RESULT_FAILED;

    // get en passant square
    int epSquare = 0;
    if (pos->ep_file)
    {
        if (color == 1)
        {
            epSquare = 15 + pos->ep_file;
        }
        else
        {
            epSquare = 39 + pos->ep_file;
        }
    }

    // convert to bitboards
    uint64_t white = 0ULL, black = 0ULL, kings = 0ULL, queens = 0ULL;
    uint64_t rooks = 0ULL, bishops = 0ULL, knights = 0ULL, pawns = 0ULL;

    white |= setMask[pos->piece_list[wK][0]];
    kings |= setMask[pos->piece_list[wK][0]];
    black |= setMask[pos->piece_list[bK][0]];
    kings |= setMask[pos->piece_list[bK][0]];

    for (int i = 0; i < pos->piece_count[wP]; i++)
    {
        white |= setMask[pos->piece_list[wP][i]];
        pawns |= setMask[pos->piece_list[wP][i]];
    }
    for (int i = 0; i < pos->piece_count[bP]; i++)
    {
        black |= setMask[pos->piece_list[bP][i]];
        pawns |= setMask[pos->piece_list[bP][i]];
    }
    for (int i = 0; i < pos->piece_count[wN]; i++)
    {
        white |= setMask[pos->piece_list[wN][i]];
        knights |= setMask[pos->piece_list[wN][i]];
    }
    for (int i = 0; i < pos->piece_count[bN]; i++)
    {
        black |= setMask[pos->piece_list[bN][i]];
        knights |= setMask[pos->piece_list[bN][i]];
    }
    for (int i = 0; i < pos->piece_count[wB]; i++)
    {
        white |= setMask[pos->piece_list[wB][i]];
        bishops |= setMask[pos->piece_list[wB][i]];
    }
    for (int i = 0; i < pos->piece_count[bB]; i++)
    {
        black |= setMask[pos->piece_list[bB][i]];
        bishops |= setMask[pos->piece_list[bB][i]];
    }
    for (int i = 0; i < pos->piece_count[wR]; i++)
    {
        white |= setMask[pos->piece_list[wR][i]];
        rooks |= setMask[pos->piece_list[wR][i]];
    }
    for (int i = 0; i < pos->piece_count[bR]; i++)
    {
        black |= setMask[pos->piece_list[bR][i]];
        rooks |= setMask[pos->piece_list[bR][i]];
    }
    for (int i = 0; i < pos->piece_count[wQ]; i++)
    {
        white |= setMask[pos->piece_list[wQ][i]];
        queens |= setMask[pos->piece_list[wQ][i]];
    }
    for (int i = 0; i < pos->piece_count[bQ]; i++)
    {
        black |= setMask[pos->piece_list[bQ][i]];
        queens |= setMask[pos->piece_list[bQ][i]];
    }

    return tb_probe_wdl(
        white, black, kings, queens, rooks, bishops, knights, pawns,
        0, 0, epSquare, (color == -1) ? true : false);
}

inline int tablebasesProbeDTZ(BOARD *pos, MOVE *bestMove, int *score, int color)
{
    // do not probe when # of pieces is greater than TB_LARGEST
    // or with castling rights
    if ((unsigned int) pos->piece_num > TB_LARGEST
        || pos->ksw || pos->ksb || pos->qsw || pos->qsb)
        return 0;

    // get en passant square
    int epSquare = 0;
    if (pos->ep_file)
    {
        if (color == 1)
        {
            epSquare = 15 + pos->ep_file;
        }
        else
        {
            epSquare = 39 + pos->ep_file;
        }
    }

    // convert to bitboards
    uint64_t white = 0ULL, black = 0ULL, kings = 0ULL, queens = 0ULL;
    uint64_t rooks = 0ULL, bishops = 0ULL, knights = 0ULL, pawns = 0ULL;

    white |= setMask[pos->piece_list[wK][0]];
    kings |= setMask[pos->piece_list[wK][0]];
    black |= setMask[pos->piece_list[bK][0]];
    kings |= setMask[pos->piece_list[bK][0]];

    for (int i = 0; i < pos->piece_count[wP]; i++)
    {
        white |= setMask[pos->piece_list[wP][i]];
        pawns |= setMask[pos->piece_list[wP][i]];
    }
    for (int i = 0; i < pos->piece_count[bP]; i++)
    {
        black |= setMask[pos->piece_list[bP][i]];
        pawns |= setMask[pos->piece_list[bP][i]];
    }
    for (int i = 0; i < pos->piece_count[wN]; i++)
    {
        white |= setMask[pos->piece_list[wN][i]];
        knights |= setMask[pos->piece_list[wN][i]];
    }
    for (int i = 0; i < pos->piece_count[bN]; i++)
    {
        black |= setMask[pos->piece_list[bN][i]];
        knights |= setMask[pos->piece_list[bN][i]];
    }
    for (int i = 0; i < pos->piece_count[wB]; i++)
    {
        white |= setMask[pos->piece_list[wB][i]];
        bishops |= setMask[pos->piece_list[wB][i]];
    }
    for (int i = 0; i < pos->piece_count[bB]; i++)
    {
        black |= setMask[pos->piece_list[bB][i]];
        bishops |= setMask[pos->piece_list[bB][i]];
    }
    for (int i = 0; i < pos->piece_count[wR]; i++)
    {
        white |= setMask[pos->piece_list[wR][i]];
        rooks |= setMask[pos->piece_list[wR][i]];
    }
    for (int i = 0; i < pos->piece_count[bR]; i++)
    {
        black |= setMask[pos->piece_list[bR][i]];
        rooks |= setMask[pos->piece_list[bR][i]];
    }
    for (int i = 0; i < pos->piece_count[wQ]; i++)
    {
        white |= setMask[pos->piece_list[wQ][i]];
        queens |= setMask[pos->piece_list[wQ][i]];
    }
    for (int i = 0; i < pos->piece_count[bQ]; i++)
    {
        black |= setMask[pos->piece_list[bQ][i]];
        queens |= setMask[pos->piece_list[bQ][i]];
    }

    unsigned probe_result = tb_probe_root(
        white, black, kings, queens, rooks, bishops, knights, pawns,
        pos->halfmove_counter, 0, epSquare, (color == -1) ? true : false, NULL);

    //  failed or finished
    if (probe_result == TB_RESULT_FAILED 
        || probe_result == TB_RESULT_CHECKMATE
        || probe_result == TB_RESULT_STALEMATE)
    {
        return 0;
    }

    // get the wdl score
    int wdl = TB_GET_WDL(probe_result);
    switch (wdl)
    {
        case TB_WIN:
            *score = 20000;
            break;
        case TB_LOSS:
            *score = -20000;
            break;
        default:
            *score = 0;
            break;
    }

    // convert fathom move and get the best move
    unsigned tb_from = TB_GET_FROM(probe_result);
    unsigned tb_to = TB_GET_TO(probe_result);
    unsigned tb_promo = TB_GET_PROMOTES(probe_result);

    bestMove->from = (56 - 16 * (tb_from / 8)) + tb_from;
    bestMove->to = (56 - 16 * (tb_to / 8)) + tb_to;
    bestMove->promotion = promotions[tb_promo];

    return 1;
}