#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>
#include <pthread.h>
#include "attack.h"
#include "search.h"
#include "board.h"
#include "move.h"
#include "movegen.h"
#include "capturegen.h"
#include "checkmove.h"
#include "ordermove.h"
#include "evaluate.h"
#include "transposition.h"
#include "see.h"
#include "syzygy.h"

#define LOWERBOUND  0
#define EXACT       1
#define UPPERBOUND  2
#define ASWINDOW    50
#define INFINITE    20000

// global variables:
struct timeval starting_time;
char pv_table[MAXDEPTH][6];
int lmr_table[64][64];

// global variables in header
int history_index;
bool stop;
double search_time;
bool ponderhit;
double ponder_time;
bool extra_time;
bool analyze;
bool time_management;
bool node_mode;
int search_nodes;
bool stop_search;

inline void init_lmr()
{
    for (int depth = 1; depth < 64; depth++)
    {
        for (int move = 1; move < 64; move++)
        {
            lmr_table[depth][move] = 1 + log(depth) * log(move) / 2;
        }
    }
}

// save killer moves
static inline void saveKiller(MOVE move, int ply, THREAD *thread)
{
    // make sure killer moves are different
    if (!compareMove(&(thread->killers[ply][0]), &move))
    {
        thread->killers[ply][1] = thread->killers[ply][0];
    }
    // save killer move
    thread->killers[ply][0] = move;
}

// clear killer moves table
inline void clearKiller(THREAD *thread)
{
    for (int x = 0; x < MAXDEPTH; x++)
    {
        clear_move(&(thread->killers[x][0]));
        clear_move(&(thread->killers[x][1]));
    }
}

// save data in the counter move heuristic table for move ordering
static inline void saveCounterMove(THREAD *thread, BOARD *pos, MOVE move)
{
    if (pos->last_move.piece != NOMOVE)
    {
        thread->counterMoves[pos->last_move.piece][pos->last_move.to].piece = pos->board[move.from / 8][move.from % 8];
        thread->counterMoves[pos->last_move.piece][pos->last_move.to].to = move.to;
    }
}

// clear counter move table
inline void clearCounterMoveTable(THREAD *thread)
{
    for (int a = 0; a < 12; a++)
    {
        for (int b = 0; b < 64; b++)
        {
            thread->counterMoves[a][b].piece = NOMOVE;
            thread->counterMoves[a][b].to = NOMOVE;
        }
    }
}

// age history heuristic array
inline void ageHistory(THREAD *thread)
{
    for (int x = 0; x < 2; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            for (int z = 0; z < 64; z++)
            {
                thread->history[x][y][z] = thread->history[x][y][z] / 8;
            }
        }
    }
}

// prevent history heuristic array to overflow 
static inline void preventOverflow(THREAD *thread)
{
    for (int d = 0; d < 2; d++)
    {
        for (int e = 0; e < 64; e++)
        {
            for (int f = 0; f < 64; f++)
            {
                thread->history[d][e][f] = thread->history[d][e][f] / 2;
            }
        }
    }
}

// save data in the history heuristic table for move ordering
static inline void saveHistory(MOVE move, int depth, int color, THREAD *thread)
{
    int a = (color == 1) ? 1 : 0;
    int b = move.from;
    int c = move.to;

    // increase score for this move
    thread->history[a][b][c] += depth * depth;

    // prevent overflow
    if (thread->history[a][b][c] > 80000000)
    {
        preventOverflow(thread);
    }
}

static inline void adjustHistory(MOVE moveList[256], int moveIndex, int depth, int color, THREAD *thread)
{
    int a = (color == 1) ? 1 : 0;
    int b, c;

    // loop through previous moves and reduce scores
    for (int i = 0; i < moveIndex; i++)
    {
        if (!moveList[i].move_type) // quiet
        {
            b = moveList[i].from;
            c = moveList[i].to;

            thread->history[a][b][c] -= depth * depth;
            
            // prevent overflow
            if (thread->history[a][b][c] < -80000000)
            {
                preventOverflow(thread);
            }
        }
    }
}

// check for time up or stop command from GUI every 1024 nodes
static inline void timeUp(THREAD *thread)
{
    // only check for main thread
    if (!stop_search && !(thread->nodes & 1023) && thread->index == MAINTHREAD)
    {
        struct timeval ending_time;
        gettimeofday(&ending_time, NULL);
        double secs = (double)(ending_time.tv_usec - starting_time.tv_usec) / 1000000
            + (double)(ending_time.tv_sec - starting_time.tv_sec);
        if (secs >= search_time || stop == true || (ponderhit && secs >= ponder_time))
        {
            stop_search = true;
        }
        if (node_mode && getTotalNodes() >= search_nodes)
        {
            stop_search = true;
        }
    }
}

static inline bool check_repetition(THREAD *thread, unsigned long long key, int counter, int ply)
{
    // store the current position key into the history table
    thread->history_log[history_index + ply] = key;
    // check positions till a capture or a pawn move
    for (int x = history_index + ply - 2; x >= history_index + ply - counter; x -= 2)
    {
        if (key == thread->history_log[x])
        {
            return true;
        }
    }

    return false;
}

// contempt factor
// draw contempt factor of -0.25 pawn
static inline int contempt(BOARD *pos, int ply)
{
    if (pos->piece_num <= 14)
    {
        return 0;
    }

    if ((ply % 2) == 0)
        return -25;

    return 25;
}

// check if the side to move has any non-pawn materials
static inline bool nonPawnMaterial(BOARD *pos, int color)
{
    // black
    if (color == 1)
    {
        if (pos->piece_count[bN] || pos->piece_count[bB] || pos->piece_count[bR] || pos->piece_count[bQ])
        {
            return true;
        }
    }
    // white
    else
    {
        if (pos->piece_count[wN] || pos->piece_count[wB] || pos->piece_count[wR] || pos->piece_count[wQ])
        {
            return true;
        }
    }

    return false;
}

// fail-soft quiescence search with captures and queen promotion
static int quiescence(THREAD *thread, BOARD *pos, int ply, int color, int alpha, int beta)
{
    int value = -INFINITE;
    int length;
    int isprom;
    unsigned char moved_piece, piece;
    int moved_piece_value;
    int cap_piece_value;
    int new_x, new_y;
    BOARD pos_copy;
    struct DataItem *entry;

    // prefetch hash table
    __builtin_prefetch(&tt[pos->key % (HASHSIZE - 1)]);

    // exit if time is up
    timeUp(thread);
    if (stop_search)
        return 0;

    thread->nodes++;

    int TTeval;
    short eval = VALUENONE;
    // transposition table look up
    entry = probeTT(pos->key);
    if (entry != NULL)
    {
        // adjust mate scores form tt
        TTeval = valueFromTT(entry->evaluation, ply);
        
        // cutoff based on the flag
        switch (entry->flag)
        {
            case EXACT:
                return TTeval;
            case LOWERBOUND:
                if (beta <= TTeval)
                    return TTeval;
                break;
            case UPPERBOUND:
                if (alpha >= TTeval)
                    return TTeval;
                break;
        }

        // get static eval from tt
        eval = entry->statEval;
    }

    int standing_pat = ((eval != VALUENONE)? eval : evaluate(thread, pos, pos->board, color));

    if (standing_pat >= beta)
    {
        return standing_pat;
    }

    // delta pruning
    if (standing_pat + 975 < alpha)
    {
        return alpha;
    }

    if (standing_pat > alpha)
    {
        alpha = standing_pat;
    }

    int best = standing_pat;

    // generate captures and queen promotion
    MOVE moves[256];
    int scores[256];
    length = captureGen(pos, moves, scores, color);

    for (int x = 0; x < length; x++)
    {
        // find the move with highest score
        movesort(moves, scores, length, x);
        // make a copy of the board
        pos_copy = *pos;
        isprom = makeMove_qsearch(&pos_copy, &moves[x]);

        // skip if the king is left in check
        if (ifCheck(&pos_copy, color))
        {
            continue;
        }

        // get piece value
        new_x = moves[x].to / 8;
        new_y = moves[x].to % 8;
        piece = pos->board[new_x][new_y];
        cap_piece_value = piece_value[piece];

        // futility pruning
        if ((standing_pat + cap_piece_value + 200) < alpha && !isprom)
        {
            if (pos_copy.piece_num > 10)
                continue;
        }

        moved_piece = pos_copy.board[new_x][new_y];
        moved_piece_value = piece_value[moved_piece];

        // SEE pruning
        // only check when higher takes lower
        // do not check when king is capturing or promotion
        if (moved_piece_value > cap_piece_value && moved_piece_value != INFINITE && !isprom)
        {
            if (SEE(pos_copy.board, new_x, new_y, cap_piece_value, color) < 0)
                continue;
        }

        value = -quiescence(thread, &pos_copy, ply + 1, -color, -beta, -alpha);

        if (stop_search)
            return 0;

        if (value > best)
        {
            best = value;

            if (value > alpha)
            {
                alpha = value;

                if (value >= beta)
                {
                    break; // beta cut-off
                }
            }
        }
    }

    return best;
}

// fail-soft principal variation search
static int pvs(THREAD *thread, BOARD *pos, int depth, int ply, int color, int alpha, int beta, bool DoNull, bool is_PV, int isCheck)
{
    int value = -INFINITE;
    int best = -INFINITE;
    int length;
    struct DataItem *entry;
    int entryFlag = UPPERBOUND;
    int mate_value = INFINITE - ply;
    bool futility = false;
    int moves_made = 0, quietCount = 0;
    int reduction_depth, new_depth;
    int isTactical, giveCheck, hist_score;
    BOARD pos_copy;
    MOVE bm, hash_move;
    int extension, hashMoveType;
    int probcutVal, probcutBeta;
    unsigned tbprobe;
    int tbValue, tbBound;
    bm.from = NOMOVE;
    hash_move.from = NOMOVE;

    // prefetch hash table
    __builtin_prefetch(&tt[pos->key % (HASHSIZE - 1)]);

    // exit if time is up
    timeUp(thread);
    if (stop_search)
        return 0;

    // mate distance pruning
    if (alpha < -mate_value)
        alpha = -mate_value;
    if (beta > mate_value - 1)
        beta = mate_value - 1;
    if (alpha >= beta)
        return alpha;

    // do not drop to qsearch when in check
    if (isCheck && depth == 0)
    {
        depth++;
    }

    // check draw of repetition
    if (check_repetition(thread, pos->key, pos->halfmove_counter, ply))
    {
        thread->nodes++;
        return contempt(pos, ply);
    }

    // if depth reaches the end
    if (depth == 0)
    {
        return quiescence(thread, pos, ply, color, alpha, beta);
    }

    thread->nodes++;

    int TTeval;
    short eval = VALUENONE;
    // transposition table look up
    entry = probeTT(pos->key);
    if (entry != NULL)
    {
        if (entry->depth >= depth)
        {
            // adjust mate scores form tt
            TTeval = valueFromTT(entry->evaluation, ply);

            // cutoff based on the flag
            switch (entry->flag)
            {
                case EXACT:
                    return TTeval;
                case LOWERBOUND:
                    if (beta <= TTeval)
                        return TTeval;
                    break;
                case UPPERBOUND:
                    if (alpha >= TTeval)
                        return TTeval;
                    break;
            }
        }

        // get the hash move when it's not upperbound
        if (entry->flag != UPPERBOUND)
            hash_move = entry->bestmove;
        // get static eval from tt
        eval = entry->statEval;
    }

    // probe Syzygy tablebases
    if ((tbprobe = tablebasesProbeWDL(pos, depth, color)) != TB_RESULT_FAILED)
    {
        thread->tbhits++;

        switch (tbprobe)
        {
            case TB_WIN:
                tbValue = INFINITE - MAXDEPTH - ply;
                tbBound = LOWERBOUND;
                break;
            case TB_LOSS:
                tbValue = -INFINITE + MAXDEPTH + ply;
                tbBound = UPPERBOUND;
                break;
            default:
                tbValue = 0;
                tbBound = EXACT;
                break;
        }

        if (tbBound == EXACT 
            || (tbBound == LOWERBOUND && tbValue >= beta)
            || (tbBound == UPPERBOUND && tbValue <= alpha))
        {
            storeTT(pos->key, valueToTT(tbValue, ply), eval, depth, &bm, tbBound);
            return tbValue;
        }
    }

    // skip forward pruning when it's a pv node or in check 
    if (is_PV || isCheck)
        goto skip_pruning;

    // get static eval
    if (eval == VALUENONE)
    {
        eval = evaluate(thread, pos, pos->board, color);
    }

    // razoring
    if (depth == 1 && eval <= alpha - 300)
    {
        return quiescence(thread, pos, ply, color, alpha, beta);
    }

    // static null move / reverse futility pruning
    if (depth <= 7 && abs(beta) < 19000)
    {
        int margin = 96 * depth;
        if (eval - margin >= beta)
        {
            return eval;
        }
    }

    // null move pruning
    if (DoNull && depth >= 3 && eval >= beta)
    {
        if (nonPawnMaterial(pos, color))
        {
            int R = 3 + depth / 4;

            // make null move
            pos_copy = *pos;
            make_nullmove(&pos_copy);

            int nullVal = -pvs(thread, &pos_copy, depth - R, ply + 1, -color, -beta, -beta + 1, false, false, 0);

            if (stop_search)
                return 0;

            if (nullVal >= beta)
            {
                return beta;
            }
        }
    }

    // probcut
    if (depth >= 5 && abs(beta) < 19000 && eval >= beta)
    {
        int moved_piece_value, cap_piece_value;
        int new_x, new_y;
        MOVE moves[256];
        int scores[256];

        probcutBeta = beta + 80;
        length = captureGen(pos, moves, scores, color);
        // try tactical moves
        for (int x = 0; x < length; x++)
        {
            // find the move with highest score
            movesort(moves, scores, length, x);
            // make a copy of the board
            pos_copy = *pos;
            isTactical = makeMove(&pos_copy, &moves[x]);

            // skip if the king is left in check
            if (ifCheck(&pos_copy, color))
            {
                continue;
            }

            // get piece value
            new_x = moves[x].to / 8;
            new_y = moves[x].to % 8;
            cap_piece_value = piece_value[pos->board[new_x][new_y]];
            moved_piece_value = piece_value[pos_copy.board[new_x][new_y]];

            // skip moves with SEE < 0
            if (moved_piece_value > cap_piece_value && moved_piece_value != INFINITE && isTactical != 2)
            {
                if (SEE(pos_copy.board, new_x, new_y, cap_piece_value, color) < 0)
                    continue;
            }

            // verify first with qsearch
            probcutVal = -quiescence(thread, &pos_copy, ply + 1, -color, -probcutBeta, -probcutBeta + 1);

            if (probcutVal >= probcutBeta)
            {   
                // reduced depth search to confirm the value is above beta
                giveCheck = ifCheck(&pos_copy, -color);
                probcutVal = -pvs(thread, &pos_copy, depth - 4, ply + 1, -color, -probcutBeta, -probcutBeta + 1, true, false, giveCheck);
            }
            if (probcutVal >= probcutBeta)
            {
                return probcutVal;
            }
        }
    }

    // conditions for futility pruning
    if ((eval + futilityMargin[depth]) <= alpha && abs(alpha) < 19000)
    {
        futility = true;
    }

skip_pruning:

    // internal iterative deepening
    if (is_PV && !isCheck && depth >= 6 && hash_move.from == NOMOVE)
    {
        hash_move = internalID(thread, pos, depth - depth / 4 - 1, ply, color, alpha, beta);
    }

    // initialize for move generation
    length = 256;
    MOVE moves[256];
    int scores[256];
    int start_moveGen = 0;
    // try hash move first
    if (hash_move.from != NOMOVE && isPseudoLegal(pos, &hash_move, color))
    {
        moves[0] = hash_move;
        scores[0] = HASHMOVE;
        start_moveGen = 1;
    }

    for (int x = 0; x < length; x++)
    {
        // generate moves after hash move or if hash move doesn't exist
        if (x == start_moveGen)
        {
            if (start_moveGen)
                hashMoveType = moves[0].move_type;
            length = moveGen(thread, pos, moves, scores, ply, color);
            skipHashMove(moves, scores, length, &hash_move, hashMoveType, x);
        }

        // find the move with highest score
        if (x >= start_moveGen)
            movesort(moves, scores, length, x);
        // make a copy of the board
        pos_copy = *pos;
        isTactical = makeMove(&pos_copy, &moves[x]);

        // skip if the king is left in check
        if (ifCheck(&pos_copy, color))
        {
            continue;
        }

        giveCheck = ifCheck(&pos_copy, -color);

        if (!is_PV && !isCheck && !giveCheck && depth <= 8 && !isTactical && best > -19000) // not mated
        {
            // futility pruning
            if (futility)
            {
                continue;
            }

            // late move pruning
            if (!pos_copy.pawn_push && quietCount > lmpMargin[depth])
            {
                continue;
            }
        }

        moves_made++;
        if (!isTactical)
            quietCount++;

        extension = 0;
        // passed pawn extension
        if (pos_copy.piece_num <= 18 && pos_copy.pawn_push)
        {
            extension = 1;
        }
        // check extension
        else if (giveCheck)
        {
            // don't extend checks with negative SEE
            if (isTactical)
            {
                if (scores[x] >= ECAPTURE)
                    extension = 1;
            }
            else
            {
                if (SEE(pos_copy.board, moves[x].to / 8, moves[x].to % 8, 0, color) >= 0)
                    extension = 1;
            }
        }

        // late move reduction
        reduction_depth = 0;
        new_depth = depth - 1 + extension;

        if (new_depth >= 2
            && moves_made > 3
            && !isCheck
            && !isTactical)
        {
            reduction_depth = lmr_table[MIN(new_depth, 63)][MIN(moves_made, 63)];

            hist_score = thread->history[(color == 1) ? 1 : 0][moves[x].from][moves[x].to];

            // reduce less for pv nodes
            if (is_PV)
                reduction_depth--;

            // reduce less for killer and counter moves
            if (scores[x] == KILLER1 || scores[x] == KILLER2 || scores[x] == COUNTER)
                reduction_depth--;

            // adjust reductions based on history
            if (hist_score < 0)
                reduction_depth += MIN(hist_score / -500, 2);

            reduction_depth = MIN(new_depth - 1, MAX(0, reduction_depth)); // do not drop to qsearch or extend
            new_depth -= reduction_depth;
        }

        if (moves_made == 1)
        {
            // full depth normal window search for the 1st move
            value = -pvs(thread, &pos_copy, new_depth, ply + 1, -color, -beta, -alpha, true, is_PV, giveCheck);
        }
        else
        {
            // null window search
            value = -pvs(thread, &pos_copy, new_depth, ply + 1, -color, -alpha - 1, -alpha, true, false, giveCheck);

            // search again with null window and full depth if lmr failed
            if (reduction_depth && value > alpha)
            {
                new_depth += reduction_depth;
                value = -pvs(thread, &pos_copy, new_depth, ply + 1, -color, -alpha - 1, -alpha, true, false, giveCheck);
            }

            // full depth normal window search
            if (value > alpha && value < beta)
            {
                value = -pvs(thread, &pos_copy, new_depth, ply + 1, -color, -beta, -alpha, true, true, giveCheck);
            }
        }

        if (stop_search)
            return 0;

        if (value > best)
        {
            best = value;
            bm = moves[x];

            if (value > alpha)
            {
                alpha = value;
                entryFlag = EXACT;

                if (value >= beta)
                {
                    if (!isTactical)
                    {
                        saveKiller(moves[x], ply, thread);
                        saveCounterMove(thread, pos, moves[x]);
                        saveHistory(moves[x], depth, color, thread);
                    }
                    if (quietCount)
                    {
                        adjustHistory(moves, x, depth, color, thread);
                    }
                    entryFlag = LOWERBOUND;
                    break; // beta cut-off
                }
            }
        }
    }

    if (!moves_made)
    {
        if (isCheck)
        {
            bm.from = NOMOVE;
            best = -INFINITE + ply; // checkmate
        }
        else
        {
            return contempt(pos, ply); // stalemate
        }
    }
    // draw by fifty moves
    else if (pos->halfmove_counter >= 100)
    {
        return contempt(pos, ply);
    }

    // transposition table store:    
    storeTT(pos->key, valueToTT(best, ply), eval, depth, &bm, entryFlag);
    return best;
}

// internal iterative deepening
MOVE internalID(THREAD *thread, BOARD *pos, int depth, int ply, int color, int alpha, int beta)
{
    int value = -INFINITE;
    int best = -INFINITE;
    int length;
    int entryFlag = UPPERBOUND;
    int reduction_depth, new_depth;
    int isTactical, giveCheck, hist_score;
    int moves_made = 0, quietCount = 0;
    BOARD pos_copy;
    MOVE bm;
    bm.from = NOMOVE;

    // exit if time is up
    timeUp(thread);
    if (stop_search)
        return bm;

    thread->nodes++;

    // generate moves
    MOVE moves[256];
    int scores[256];
    length = moveGen(thread, pos, moves, scores, ply, color);

    for (int x = 0; x < length; x++)
    {
        // find the move with highest score
        movesort(moves, scores, length, x);
        // make a copy of the board
        pos_copy = *pos;
        isTactical = makeMove(&pos_copy, &moves[x]);

        // skip if the king is left in check
        if (ifCheck(&pos_copy, color))
        {
            continue;
        }

        moves_made++;
        if (!isTactical)
            quietCount++;

        giveCheck = ifCheck(&pos_copy, -color);

        // late move reduction
        reduction_depth = 0;
        new_depth = depth - 1;

        if (new_depth >= 2
            && moves_made > 3
            && !giveCheck
            && !isTactical)
        {
            reduction_depth = lmr_table[MIN(new_depth, 63)][MIN(moves_made, 63)];

            hist_score = thread->history[(color == 1) ? 1 : 0][moves[x].from][moves[x].to];

            // reduce less for pv nodes
            reduction_depth--;

            // reduce less for killer and counter moves
            if (scores[x] == KILLER1 || scores[x] == KILLER2 || scores[x] == COUNTER)
                reduction_depth--;

            // adjust reductions based on history
            if (hist_score < 0)
                reduction_depth += MIN(hist_score / -500, 2);

            reduction_depth = MIN(new_depth - 1, MAX(0, reduction_depth)); // do not drop to qsearch or extend
            new_depth -= reduction_depth;
        }

        if (moves_made == 1)
        {
            // full depth normal window search for the 1st move
            value = -pvs(thread, &pos_copy, new_depth, ply + 1, -color, -beta, -alpha, true, true, giveCheck);
        }
        else
        {
            // null window search
            value = -pvs(thread, &pos_copy, new_depth, ply + 1, -color, -alpha - 1, -alpha, true, false, giveCheck);

            // search again with null window and full depth if lmr failed
            if (reduction_depth && value > alpha)
            {
                new_depth += reduction_depth;
                value = -pvs(thread, &pos_copy, new_depth, ply + 1, -color, -alpha - 1, -alpha, true, false, giveCheck);
            }

            // full depth normal window search
            if (value > alpha && value < beta)
            {
                value = -pvs(thread, &pos_copy, new_depth, ply + 1, -color, -beta, -alpha, true, true, giveCheck);
            }
        }

        // exit if time is up
        if (stop_search)
            return bm;

        if (value > best)
        {
            best = value;

            if (value > alpha)
            {
                alpha = value;
                bm = moves[x];
                entryFlag = EXACT;

                if (value >= beta)
                {
                    if (!isTactical)
                    {
                        saveKiller(moves[x], ply, thread);
                        saveCounterMove(thread, pos, moves[x]);
                        saveHistory(moves[x], depth, color, thread);
                    }
                    if (quietCount)
                    {
                        adjustHistory(moves, x, depth, color, thread);
                    }
                    entryFlag = LOWERBOUND;
                    break; // beta cut-off
                }
            }
        }
    }

    if (!moves_made)
    {
        bm.from = NOMOVE;
        return bm; // stalemate
    }

    // transposition table store:  
    storeTT(pos->key, valueToTT(best, ply), VALUENONE, depth, &bm, entryFlag);
    return bm;
}

// principal variation search at root
static int pvs_root(THREAD *thread, int depth, int color, int alpha, int beta)
{
    int value = -INFINITE;
    int best = -INFINITE;
    int length;
    struct DataItem *entry;
    int entryFlag = UPPERBOUND;
    int reduction_depth, new_depth;
    int isTactical, giveCheck, hist_score;
    int moves_made = 0, quietCount = 0;
    BOARD *pos = &thread->pos;
    BOARD pos_copy;
    MOVE bm, hash_move;
    bm.from = NOMOVE;
    clear_move(&hash_move);

    thread->nodes++;

    // transposition table look up
    entry = probeTT(pos->key);
    if (entry != NULL && entry->flag != UPPERBOUND)
    {
        hash_move = entry->bestmove;
    }

    // generate moves
    MOVE moves[256];
    int scores[256];
    // get the best move from last iteration if any
    MOVE pv_move = string_to_move(thread->BestMove);

    length = moveGen(thread, pos, moves, scores, 0, color);
    int sorted = orderMove_root(moves, scores, length, &pv_move, &hash_move);

    for (int x = 0; x < length; x++)
    {
        // find the move with highest score
        if (x >= sorted)
            movesort(moves, scores, length, x);
        // make a copy of the board
        pos_copy = *pos;
        isTactical = makeMove(&pos_copy, &moves[x]);

        // skip if the king is left in check
        if (ifCheck(&pos_copy, color))
        {
            continue;
        }

        moves_made++;
        if (!isTactical)
            quietCount++;

        giveCheck = ifCheck(&pos_copy, -color);

        // late move reduction at root
        reduction_depth = 0;
        new_depth = depth - 1;

        if (new_depth >= 2
            && moves_made > 5
            && !giveCheck
            && !isTactical)
        {
            reduction_depth = lmr_table[MIN(new_depth, 63)][MIN(moves_made, 63)];

            hist_score = thread->history[(color == 1) ? 1 : 0][moves[x].from][moves[x].to];

            // reduce less for pv nodes
            reduction_depth--;

            // reduce less for killer and counter moves
            if (scores[x] == KILLER1 || scores[x] == KILLER2 || scores[x] == COUNTER)
                reduction_depth--;

            // adjust reductions based on history
            if (hist_score < 0)
                reduction_depth += MIN(hist_score / -500, 2);

            reduction_depth = MIN(new_depth - 1, MAX(0, reduction_depth)); // do not drop to qsearch or extend
            new_depth -= reduction_depth;
        }

        if (moves_made == 1)
        {
            // full depth normal window search for the 1st move
            value = -pvs(thread, &pos_copy, new_depth, 1, -color, -beta, -alpha, true, true, giveCheck);
        }
        else
        {
            // null window search
            value = -pvs(thread, &pos_copy, new_depth, 1, -color, -alpha - 1, -alpha, true, false, giveCheck);

            // search again with null window and full depth if lmr failed
            if (reduction_depth && value > alpha)
            {
                new_depth += reduction_depth;
                value = -pvs(thread, &pos_copy, new_depth, 1, -color, -alpha - 1, -alpha, true, false, giveCheck);
            }

            // full depth normal window search
            if (value > alpha && value < beta)
            {
                value = -pvs(thread, &pos_copy, new_depth, 1, -color, -beta, -alpha, true, true, giveCheck);
            }
        }

        // exit if time is up
        if (stop_search)
            break;

        if (value > best)
        {
            best = value;

            if (value > alpha)
            {
                alpha = value;
                bm = moves[x];
                entryFlag = EXACT;

                if (value >= beta)
                {
                    if (!isTactical)
                    {
                        saveKiller(moves[x], 0, thread);
                        saveCounterMove(thread, pos, moves[x]);
                        saveHistory(moves[x], depth, color, thread);
                    }
                    if (quietCount)
                    {
                        adjustHistory(moves, x, depth, color, thread);
                    }
                    entryFlag = LOWERBOUND;
                    break; // beta cut-off
                }
            }
        }
    }

    thread->searched_move = bm; // save the best move to play
    // transposition table store:  
    storeTT(pos->key, best, VALUENONE, depth, &bm, entryFlag);
    return best;
}

// obtain the principal variation from the hash table
static inline void getPVline(BOARD *pos, MOVE *bestmove, int depth)
{
    struct DataItem *item;
    MOVE smove;
    BOARD pos_copy;
    char move[6];

    // make a copy
    pos_copy = *pos;
    smove = *bestmove;
    move_to_string(bestmove, move);
    strncpy(pv_table[0], move, 6);
    pv_table[0][5] = '\0';

    for (int x = 1; x < depth; x++)
    {
        if (smove.from == NOMOVE)
        {
            return;
        }
        makeMove(&pos_copy, &smove);

        item = probeTT(pos_copy.key);
        if (item != NULL)
        {
            move_to_string(&item->bestmove, move);
            strncpy(pv_table[x], move, 6);
            pv_table[x][5] = '\0';
            smove = item->bestmove;
        }
        else
        {
            break;
        }
    }
}

// get nodes per second
static inline int get_nps(int node_count, double secs)
{
    if (secs == 0)
        return 0;

    return (int)(node_count / secs);
}

// check if the destinations of these two moves are the same
static inline bool isRecapture(char move[6], char op_move[6])
{
    if (move[2] == op_move[2] && move[3] == op_move[3])
    {
        return true;
    }

    return false;
}

// iterative deepening
static void *iterative_deepening(void *arg)
{
    int current_depth;
    int alpha = -INFINITE;
    int beta = INFINITE;
    int val;
    struct timeval ending_time;
    double secs;
    bool more_time = true;
    bool valid_partial_search = false;
    bool failed_low = false;
    bool eazy_move = true;
    char move[6];
    THREAD *thread = arg;
    BOARD *pos = &thread->pos;
    int depth = thread->depth;
    int color = thread->color;
    int nodes, tbhits;

    for (current_depth = 1; current_depth <= depth; current_depth++)
    {
        clear_move(&thread->searched_move);
        // search starts
        val = pvs_root(thread, current_depth, color, alpha, beta);

        // check time
        gettimeofday(&ending_time, NULL);
        secs = (double)(ending_time.tv_usec - starting_time.tv_usec) / 1000000
            + (double)(ending_time.tv_sec - starting_time.tv_sec);
        if (secs >= search_time || stop == true || (ponderhit && secs >= ponder_time))
        {
            stop_search = true;
        }
        if (node_mode && getTotalNodes() >= search_nodes)
        {
            stop_search = true;
        }

        if (stop_search)
        {
            // allow partial search results if at least one move searched and it's within the bounds/not failed low
            // only do this in main thread
            if (thread->searched_move.from != NOMOVE && !failed_low && val > alpha && val < beta && thread->index == MAINTHREAD)
            {
                valid_partial_search = true;
            }
            else
            {
                break;
            }
        }

        // aspiration window
        if (val <= alpha)
        {
            alpha = -INFINITE;
            beta = INFINITE;
            // search longer if failed low
            // only in main thread
            if (more_time && extra_time && thread->index == MAINTHREAD)
            {
                if (secs >= (search_time / 8))
                {
                    search_time *= 1.8;
                    more_time = false;
                }
                else if (secs >= (ponder_time / 8))
                {
                    ponder_time *= 1.8;
                    more_time = false;
                }
            }
            failed_low = true;
            eazy_move = false;
            current_depth -= 1;
            continue;
        }
        else if (val >= beta)
        {
            alpha = -INFINITE;
            beta = INFINITE;
            failed_low = false;
            current_depth -= 1;
            continue;
        }

        failed_low = false;

        alpha = val - ASWINDOW;
        beta = val + ASWINDOW;

        // get best move
        move_to_string(&thread->searched_move, move);

        // If depth is not 1 and the best move from the last depth is not the same as the best move from this depth,
        // then don't reduce thinking time
        if (current_depth != 1 && strncmp(thread->BestMove, move, 5))
        {
            eazy_move = false;
        }

        // copy the best move so the next search will know
        strncpy(thread->BestMove, move, 6); // needed for every thread
        thread->BestMove[5] = '\0';

        // skip the rest if not main thread
        if (thread->index != MAINTHREAD)
            continue;

        // get the pv line
        memset(pv_table, 0, sizeof(pv_table));
        getPVline(pos, &thread->searched_move, current_depth);

        nodes = getTotalNodes();
        tbhits = getTotalTbhits();

        // send info to GUI
        if (val > 19000)
        {
            printf("info depth %d score mate %d nodes %d time %d nps %d tbhits %d pv", current_depth, (INFINITE - val - 1) / 2 + 1,
                nodes, (int)(secs * 1000), get_nps(nodes, secs), tbhits);
        }
        else if (val < -19000)
        {
            printf("info depth %d score mate %d nodes %d time %d nps %d tbhits %d pv", current_depth, -(INFINITE + val - 1) / 2 - 1,
                nodes, (int)(secs * 1000), get_nps(nodes, secs), tbhits);
        }
        else
        {
            printf("info depth %d score cp %d nodes %d time %d nps %d tbhits %d pv", current_depth, val, nodes, (int)(secs * 1000),
                get_nps(nodes, secs), tbhits);
        }

        for (int i = 0; i < current_depth; i++)
        {
            printf(" %s", pv_table[i]);
        }
        printf("\n");
        fflush(stdout);

        if (valid_partial_search)
        {
            break;
        }
        // easy move if the best move is a recapture and has not changed after depth 12
        if (time_management && eazy_move && current_depth >= 12 && secs >= (search_time / 16))
        {
            if (isRecapture(thread->BestMove, thread->op_move))
                break;
        }
    }
    
    if (thread->index == MAINTHREAD)
    {
        // send move to GUI
        if (!strncmp(thread->BestMove, "", 5))
        {
            printf("bestmove 0000\n");
        }
        else if (!strncmp(pv_table[1], "", 5))
        {
            printf("bestmove %s\n", thread->BestMove);
        }
        else
        {
            printf("bestmove %s ponder %s\n", thread->BestMove, pv_table[1]);
        }

        fflush(stdout);
    }
    
    return NULL;
}

void search(BOARD *pos, int piece_color, char op_move[6], int thread_num, unsigned long long *history_log)
{
    // get the starting time
    gettimeofday(&starting_time, NULL);

    MOVE probedMove;
    char move[6];
    int score;
    pthread_t pthreads[thread_num];

    // probe Syzygy tablebases at root
    if (!analyze && tablebasesProbeDTZ(pos, &probedMove, &score, piece_color))
    {
        move_to_string(&probedMove, move);
        printf("info depth 1 score cp %d nodes 0 time 0 nps 0 tbhits 1 pv %s\n", score, move);
        printf("bestmove %s\n", move);
        fflush(stdout);
        return;
    }

    // prepare to search
    stop_search = false;
    prepareSearch(pos, piece_color, op_move, history_log);

    // search
    // start helper threads
    for (int i = 1; i < thread_num; i++) {
        pthread_create(&pthreads[i], NULL, iterative_deepening, &threads[i]);
    }

    // use this thread as main thread
    iterative_deepening(&threads[0]);

    // stop helper threads
    stop_search = true;

    // wait for helper threads to finish
    for (int i = 1; i < thread_num; i++) {
        pthread_join(pthreads[i], NULL);
    }

    // clear tables in analyze mode
    if (analyze)
    {
        clearTT(); // clear main hash table
        clearEvalTT(); // clear evaluation hash table
        cleanupSearch();
    }
    else
    {
        setAge(); // otherwise, age tt
    }
}