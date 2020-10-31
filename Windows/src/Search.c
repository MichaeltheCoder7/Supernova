#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>
#include "Attack.h"
#include "Search.h"
#include "Board.h"
#include "Move.h"
#include "MoveGen.h"
#include "CaptureGen.h"
#include "OrderMove.h"
#include "Evaluate.h"
#include "Transposition.h"
#include "SEE.h"

#define LOWERBOUND  0
#define EXACT       1
#define UPPERBOUND  2
#define ASWINDOW    50
#define INFINITE    20000

//global variables:
int nodes;
struct timeval starting_time;
char BestMove[6];
MOVE searched_move;

//clear killer moves table
static inline void clearKiller()
{
    for(int x = 0; x < MAXDEPTH; x++)
    {
        clear_move(&killers[x][0]);
        clear_move(&killers[x][1]);
    }
}

//age history heuristic array
static inline void ageHistory()
{
    for(int x = 0; x < 2; x++)
    {
        for(int y = 0; y < 64; y++)
        {
            for(int z = 0; z < 64; z++)
            {
                history[x][y][z] = history[x][y][z] / 8;
            }
        }
    }
}

//prevent history heuristic array to overflow 
static inline void preventOverflow()
{
    for(int d = 0; d < 2; d++)
    {
        for(int e = 0; e < 64; e++)
        {
            for(int f = 0; f < 64; f++)
            {
                history[d][e][f] = history[d][e][f] / 2;
            }
        }
    }
}

//check for time up or stop command from GUI every 1024 nodes
static inline void timeUp()
{
    if(!stop_search && !(nodes & 1023))
    {
        struct timeval ending_time;
        gettimeofday(&ending_time, NULL);
        double secs = (double)(ending_time.tv_usec - starting_time.tv_usec) / 1000000 + (double)(ending_time.tv_sec - starting_time.tv_sec);
        if(secs >= search_time || stop == true || (ponderhit && secs >= ponder_time))
        {
            stop_search = true;
        }
        if(node_mode && nodes >= search_nodes)
        {
            stop_search = true;
        }
    }
}

static inline bool check_repetition(unsigned long long key, int counter, int ply)
{
    //store the current position key into the history table
    history_log[history_index+ply] = key;
    //check positions till an capture or a pawn move
    for(int x = history_index+ply - 2; x >= history_index+ply-counter; x-=2)
    {
        if(key == history_log[x])
        {
            return true;
        }
    }

    return false;
}

//contempt factor
//draw contempt factor of -0.25 pawn
static inline int contempt(BOARD *pos, int ply)
{
    if(pos->piece_num <= 14)
    {
        return 0;
    }

    if((ply % 2) == 0)
        return -25;

    return 25;
}

//check if the side to move has non-pawn material
static inline bool nonPawnMaterial(BOARD *pos, int color)
{
    //black
    if(color == 1)
    {
        if(pos->piece_count[bN] || pos->piece_count[bB] || pos->piece_count[bR] || pos->piece_count[bQ])
        {
            return true;
        }
    }
    //white
    else
    {
        if(pos->piece_count[wN] || pos->piece_count[wB] || pos->piece_count[wR] || pos->piece_count[wQ])
        {
            return true;
        }
    }
    
    return false;
}

static inline bool isCapturePromotion(BOARD *pos, MOVE *move)
{
    if(move->promotion != ' ')
    {
        return true;
    }
    else if(pos->board[move->to / 8][move->to % 8] != ' ')
    {
        return true;
    }

    return false;
}

//quiescence search with captures and queen promotion
static int quiescence(BOARD *pos, int color, int alpha, int beta)
{
    int value;
    int length;
    int isprom;
    char moved_piece, piece;
    int moved_piece_value;
    int cap_piece_value;
    int new_x, new_y;
    BOARD pos_copy;

    //prefetch hash table
    __builtin_prefetch(&Evaltt[pos->key % EVALHASHSIZE]);

    //check if time is up
    timeUp();
    if(stop_search)
        return 0;

    nodes++;

    int standing_pat = evaluate(pos, pos->board, color);

    if(standing_pat >= beta) 
    {
        return beta;
    }
    
    //delta pruning
    if(standing_pat + 900 < alpha)
    {
        return alpha;
    }

    if(standing_pat > alpha) 
    {
        alpha = standing_pat;
    }
    
    value = -INFINITE;
    //get children of node
    MOVE moves[256];
    int scores[256];
    length = captureGen(pos, moves, scores, color);

    for(int x = 0; x < length; x++)
    {
        //find the move with highest score
        movesort(moves, scores, length, x);
        //make a copy of the board
        pos_copy = *pos;
        isprom = makeMove_qsearch(&pos_copy, &moves[x]);
            
        //check if check is ignored
        if(ifCheck(&pos_copy, color))
        {
            continue;
        }
        
        //get piece value
        new_x = moves[x].to / 8;
        new_y = moves[x].to % 8;
        piece = pos->board[new_x][new_y];
        cap_piece_value = piece_value(piece);

        //delta pruning
        if((standing_pat + cap_piece_value + 200) < alpha && !isprom)
        {
            if(pos_copy.piece_num > 10)
                continue;
        }

        moved_piece = pos_copy.board[new_x][new_y];
        moved_piece_value = piece_value(moved_piece);

        //SEE pruning
        //only check when higher takes lower and not promotion
        //do not check when king is capturing
        if(moved_piece_value > cap_piece_value && moved_piece_value != INFINITE && !isprom)
        {
            if(SEE(pos_copy.board, new_x, new_y, cap_piece_value, color) < 0)
            {
                continue;
            }
        }

        value = -quiescence(&pos_copy, -color, -beta, -alpha);

        if(stop_search)
            return 0;

        if(value > alpha)
        {
            if(value >= beta)
            {
                return beta; //beta cut-off
            }
            alpha = value;
        }
    }
    
    return alpha;
}

//fail-hard principal variation search
static int pvs(BOARD *pos, int depth, int ply, int color, int alpha, int beta, bool DoNull, bool is_PV)
{   
    int value = -INFINITE;
    int best = -INFINITE;
    int length;
    struct DataItem* entry;
    int entryFlag = UPPERBOUND;
    bool alpha_raised = false;
    int mate_value = INFINITE - ply;
    bool futility = false;
    int moves_made = 0, quietCount = 0;
    int reduction_depth, new_depth;
    int isTactical;
    BOARD pos_copy;
    MOVE bm;
    MOVE hash_move;
    int extension;
    int probcutVal, probcutBeta;
    bm.from = NOMOVE;
    hash_move.from = NOMOVE;

    //prefetch hash table
    __builtin_prefetch(&tt[pos->key % (HASHSIZE - 1)]);

    //check if time is up
    timeUp();
    if(stop_search)
        return 0;

    //mate distance pruning
    if(alpha < -mate_value) 
        alpha = -mate_value;
    if(beta > mate_value - 1) 
        beta = mate_value - 1;
    if(alpha >= beta) 
        return alpha;
    
    int isCheck = ifCheck(pos, color);
    //check extension
    if(isCheck)
    {
        depth++;
    }

    //check draw of repetition
    if(check_repetition(pos->key, pos->halfmove_counter, ply))
    {
        nodes++;
        return contempt(pos, ply);
    }

    //if depth reaches the end
    if(depth == 0)
    {
       return quiescence(pos, color, alpha, beta);
    }
    
    nodes++;

    short eval = VALUENONE;
    //transposition table look up
    entry = probeTT(pos->key);
    if(entry != NULL)
    { 
        if(entry->depth >= depth)
        {
            switch(entry->flag)
            {
                case EXACT:
                {   
                    return entry->evaluation;
                    break;
                }
                case LOWERBOUND:
                {
                    if(beta <= entry->evaluation)
                        return beta;
                    break;
                }
                case UPPERBOUND:
                {
                    if(alpha >= entry->evaluation)
                        return alpha;
                    break;
                }
            }
        }
        //get the pv move
        if(entry->flag != UPPERBOUND)
            hash_move = entry->bestmove;
        //get static eval from tt
        eval = entry->statEval;
    }

    //get static eval
    if(!is_PV && !isCheck && eval == VALUENONE)
    {
        eval = evaluate(pos, pos->board, color);
    }

    //razoring
    if(!is_PV && !isCheck && depth == 1 && eval <= alpha - 300)
    {
        return quiescence(pos, color, alpha, beta);
    }
    
    //static null move pruning
    if(depth <= 7 && !is_PV && !isCheck && abs(beta) < 19000)
    {
        int margin = 96 * depth;
        if(eval - margin >= beta)
        {
            return eval - margin; //same as returning beta or eval in fail-hard
        }
    }

    //null move pruning
    //only at non-PV nodes
    if(DoNull && !isCheck && depth >= 3 && !is_PV && eval >= beta)
    {
        if(nonPawnMaterial(pos, color))
        {
            int R = 2;
            if(depth > 6)
            {
                R = 3;
            }

            //make null move
            int ep_file = make_nullmove(pos);

            int nullVal = -pvs(pos, depth - 1 - R, ply + 1, -color, -beta, -beta + 1, false, false);

            //undo null move
            undo_nullmove(pos, ep_file);
            
            if(stop_search)
                return 0;

            if(nullVal >= beta)
            {
                return beta;
            }
        }
    }
    
    //probcut
    if(!is_PV && !isCheck && depth > 4 && abs(beta) < 19000 && eval >= beta)
    {
        int moved_piece_value;
        int cap_piece_value;
        int new_x, new_y;
        int isprom;
        MOVE moves[256];
        int scores[256];

        probcutBeta = ((beta + 80) <= INFINITE)? (beta + 80):INFINITE;
        length = captureGen(pos, moves, scores, color);
        //try tactical moves
        for(int x = 0; x < length; x++)
        {
            //find the move with highest score
            movesort(moves, scores, length, x);
            //make a copy of the board
            pos_copy = *pos;
            isprom = makeMove(&pos_copy, &moves[x]);
            //check if check is ignored
            if(ifCheck(&pos_copy, color))
            {
                continue;
            }
            //get piece value
            new_x = moves[x].to / 8;
            new_y = moves[x].to % 8;
            cap_piece_value = piece_value(pos->board[new_x][new_y]);
            moved_piece_value = piece_value(pos_copy.board[new_x][new_y]);
            //SEE pruning
            if(moved_piece_value > cap_piece_value && moved_piece_value != INFINITE && isprom != 2)
            {
                if(SEE(pos_copy.board, new_x, new_y, cap_piece_value, color) < 0)
                {
                    continue;
                }
            }
            
            probcutVal = -quiescence(&pos_copy, -color, -probcutBeta, -probcutBeta + 1);

            if(probcutVal >= probcutBeta)
            {
                //reduced depth search to confirm the value is above beta
                probcutVal = -pvs(&pos_copy, depth - 4, ply + 1, -color, -probcutBeta, -probcutBeta + 1, true, false);
            }
            if(probcutVal >= probcutBeta)
            {
                return probcutVal;
            }
        }
    }

    //conditions for futility pruning
    if((eval + futilityMargin[depth]) <= alpha && abs(alpha) < 19000)
    {
        futility = true;
    }

    //get children of node
    MOVE moves[256];
    int scores[256];
    length = moveGen(pos, moves, scores, ply, color);
    int move_exist = orderHashMove(moves, scores, length, &hash_move);

    for(int x = 0; x < length; x++)
    {
        //find the move with highest score
        if(x != 0 || !move_exist)
            movesort(moves, scores, length, x);
        //make a copy of the board
        pos_copy = *pos;
        isTactical = makeMove(&pos_copy, &moves[x]);

        //check if check is ignored
        if(ifCheck(&pos_copy, color))
        {
            continue;
        }

        if(!is_PV && !isCheck && depth <= 8 && !isTactical && best > -19000) //not mated
        {
            //fultility pruning
            if(futility)
            {
                if(!ifCheck(&pos_copy, -color))
                {
                    continue;
                }
            }

            //late move pruning
            if(!pos_copy.pawn_push && quietCount > lmpMargin[depth])
            {
                if(!ifCheck(&pos_copy, -color))
                {
                    continue;
                }
            }
        }

        moves_made++;
        if(!isTactical)
            quietCount++;

        //hack to ensure pv line is more intact
        if(moves_made == 1)
            bm = moves[x];

        extension = 0;
        //passed pawn extension
        if(!isCheck && pos_copy.piece_num <= 18 && pos_copy.pawn_push)
        {
            extension = 1;
        }

        //late move reduction
        reduction_depth = 0;
        new_depth = depth - 1 + extension;

        if(new_depth >= 2
           && moves_made > 3
           && !isCheck
           && !isTactical
           && !compareMove(&killers[ply][0], &moves[x])
           && !compareMove(&killers[ply][1], &moves[x]))
        {
            if(!ifCheck(&pos_copy, -color))
            {
                reduction_depth = 1;
                if(moves_made > 6 && new_depth >= 3) //do not drop to qsearch
                    reduction_depth += 1;
                
                new_depth -= reduction_depth;
            }
        }

        search_again:

        if(!alpha_raised)
        {
            value = -pvs(&pos_copy, new_depth, ply + 1, -color, -beta, -alpha, true, is_PV);
        }
        else
        {
            value = -pvs(&pos_copy, new_depth, ply + 1, -color, -alpha - 1, -alpha, true, false);
            if(value > alpha)
            {
                value = -pvs(&pos_copy, new_depth, ply + 1, -color, -beta, -alpha, true, true);
            }
        }

        //search again if lmr failed
        if(reduction_depth && value > alpha)
        {
            new_depth += reduction_depth;
            reduction_depth = 0;
            goto search_again;
        }

        if(stop_search)
            return 0;

        if(value > best)
        {
            best = value;
            if(value > alpha)
            {
                bm = moves[x];
                if(value >= beta)
                {   
                    if(!isTactical)
                    {
                        //make sure killer moves are different
                        if(!compareMove(&killers[ply][0], &moves[x]))
                        {
                            killers[ply][1] = killers[ply][0];
                        }
                        //save killer move
                        killers[ply][0] = moves[x];
                        //save data in the history heuristic table for move ordering
                        int a = (color==1)?1:0;
                        int b = moves[x].from;
                        int c = moves[x].to;
                        history[a][b][c] += depth*depth;
                        //prevent overflow
                        if(history[a][b][c] > 80000000)
                        {
                            preventOverflow();
                        }
                    }
                    alpha = beta;
                    entryFlag = LOWERBOUND;
                    break; //beta cut-off
                }
                alpha_raised = true;
                alpha = value;
                entryFlag = EXACT;
            }
        }
    }

    if(!moves_made)
    {
        bm.from = NOMOVE;
        if(isCheck)
        {
            alpha = -INFINITE + ply; //checkmate
        }
        else
        {
            alpha = contempt(pos, ply); //stalemate
        }
    }
    //draw by fifty moves
    else if(pos->halfmove_counter >= 100)
    {
        return contempt(pos, ply);
    }

    //transposition table store:    
    storeTT(pos->key, alpha, eval, depth, &bm, entryFlag);
    return alpha;
}

//principal variation search at root
static int pvs_root(BOARD *pos, int depth, int color, int alpha, int beta)
{   
    int value = -INFINITE;
    int length;
    struct DataItem* entry;
    int entryFlag = UPPERBOUND;
    int isTactical;
    BOARD pos_copy;
    MOVE bm;
    MOVE hash_move;
    bm.from = NOMOVE;
    hash_move.from = NOMOVE;

    int isCheck = ifCheck(pos, color);
    //check extension
    if(isCheck)
    {
        depth++;
    }

    //transposition table look up
    entry = probeTT(pos->key);
    if(entry != NULL && entry->flag != UPPERBOUND)
    { 
        hash_move = entry->bestmove;
    }

    //get children of node
    MOVE moves[256];
    int scores[256];
    //get the best move from last iteration if any
    MOVE pv_move = string_to_move(BestMove);

    length = moveGen(pos, moves, scores, 0, color);
    orderMove_root(moves, scores, length, &pv_move, &hash_move);

    for(int x = 0; x < length; x++)
    {
        //find the move with highest score
        movesort(moves, scores, length, x);
        //make a copy of the board
        pos_copy = *pos;
        isTactical = makeMove(&pos_copy, &moves[x]);
        
        //check if check is ignored
        if(ifCheck(&pos_copy, color))
        {
            continue;
        }
        
        if(value == -INFINITE)
        {
            value = -pvs(&pos_copy, depth - 1, 1, -color, -beta, -alpha, true, true);
        }
        else
        {
            value = -pvs(&pos_copy, depth - 1, 1, -color, -alpha - 1, -alpha, true, false);
            if(value > alpha)
            {
                value = -pvs(&pos_copy, depth - 1, 1, -color, -beta, -alpha, true, true);
            }
        }
        
        //check if time is up
        if(stop_search)
            break;
        
        if(value > alpha)
        {
            bm = moves[x];
            if(value >= beta)
            {   
                if(!isTactical)
                {
                    //make sure killer moves are different
                    if(!compareMove(&killers[0][0], &moves[x]))
                    {
                        killers[0][1] = killers[0][0];
                    }
                    //save killer move
                    killers[0][0] = moves[x];
                    //save data in the history heuristic table for move ordering
                    int a = (color==1)?1:0;
                    int b = moves[x].from;
                    int c = moves[x].to;
                    history[a][b][c] += depth*depth;
                    //prevent overflow
                    if(history[a][b][c] > 80000000)
                    {
                        preventOverflow();
                    }
                }
                alpha = beta;
                entryFlag = LOWERBOUND;
                break; //beta cut-off
            }
            alpha = value;
            entryFlag = EXACT;
        }
    }
  
    searched_move = bm; //save the best move
    //transposition table store:  
    storeTT(pos->key, alpha, VALUENONE, depth, &bm, entryFlag);
    return alpha;
}

//obtain the principal variation from the hash table
static inline void getPVline(BOARD *pos, MOVE *bestmove, int depth)
{
    struct DataItem* item;
    MOVE smove;
    BOARD pos_copy;
    char move[6];
    //make a copy
    pos_copy = *pos; 
    smove = *bestmove;
    move_to_string(bestmove, move);
    strncpy(pv_table[0], move, 6);
    pv_table[0][5] = '\0';
    
    for(int x = 1; x < depth; x++)
    {   
        if(smove.from == NOMOVE)
        {
            return;
        }
        makeMove(&pos_copy, &smove);

        item = probeTT(pos_copy.key);
        if(item != NULL)
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

//get nodes per second
static inline int get_nps(int nodes, double secs)
{
    if(secs == 0)
        return 0;
    
    return (int)(nodes / secs);
}

//check if the destinations of these two moves are the same
static inline bool isRecapture(char move[6], char op_move[6])
{
    if(move[2] == op_move[2] && move[3] == op_move[3])
    {
        return true;
    }
    
    return false;
}

//iterative deepening
static void iterative_deepening(BOARD *pos, int depth, int color, char op_move[6])
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

    for(current_depth = 1; current_depth <= depth; current_depth++)
    {   
        clear_move(&searched_move);
        //search starts
        val = pvs_root(pos, current_depth, color, alpha, beta);
        
        //check time
        gettimeofday(&ending_time, NULL);
        secs = (double)(ending_time.tv_usec - starting_time.tv_usec) / 1000000 + (double)(ending_time.tv_sec - starting_time.tv_sec);
        if(secs >= search_time || stop == true || (ponderhit && secs >= ponder_time))
        {
            stop_search = true;
        }
        if(node_mode && nodes >= search_nodes)
        {
            stop_search = true;
        }

        if(stop_search)
        {
            //allow partial search results if at least one move searched and it's within the bounds/not failed low
            if(searched_move.from != NOMOVE && !failed_low && val > alpha && val < beta)
            {
                valid_partial_search = true;
            }
            else
            {
                break;
            }
        }

        //aspiration window
        if(val <= alpha)
        {
            alpha = -INFINITE;
            beta = INFINITE;
            //search longer if failed low
            if(more_time && extra_time)
            {
                if(secs >= (search_time / 8))
                {
                    search_time *= 1.8;
                    more_time = false;
                }
                else if(secs >= (ponder_time / 8))
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
        else if(val >= beta)
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

        //get the pv line and best move
        memset(pv_table, 0, sizeof(pv_table));
        getPVline(pos, &searched_move, current_depth);
        move_to_string(&searched_move, move);
        if(current_depth != 1 && strncmp(BestMove, move, 5))
        {
            eazy_move = false;
        }
        strncpy(BestMove, move, 6);
        BestMove[5] = '\0';

        //send info to gui
        if(val > 19000)
        {
            printf("info depth %d score mate %d nodes %d time %d nps %d pv", current_depth, (INFINITE - val - 1) / 2 + 1, nodes, (int)(secs*1000),
                   get_nps(nodes, secs));
        }
        else if(val < -19000)
        {
            printf("info depth %d score mate %d nodes %d time %d nps %d pv", current_depth, -(INFINITE + val - 1) / 2 - 1, nodes, (int)(secs*1000),
                   get_nps(nodes, secs));
        }
        else
        {
            printf("info depth %d score cp %d nodes %d time %d nps %d pv", current_depth, val, nodes, (int)(secs*1000), get_nps(nodes, secs));
        }

        for(int i = 0; i < current_depth; i++)
        {
            printf(" %s", pv_table[i]);
        }
        printf("\n");

        if(valid_partial_search)
        {
            break;
        }
        //easy move if the best move is a recapture and has not changed at depth 12
        if(time_management && eazy_move && current_depth == 12)
        {
            if(isRecapture(BestMove, op_move))
                break;
        }
    }
    //send move to gui
    if(!strncmp(BestMove, "", 5))
    {
        printf("bestmove 0000\n");
    }
    else if(!strncmp(pv_table[1], "", 5))
    {
        printf("bestmove %s\n", BestMove);
    }
    else
    {
        printf("bestmove %s ponder %s\n", BestMove, pv_table[1]);
    }
}

void search(BOARD *pos, int piece_color, char op_move[6])
{
    //prepare to search
    gettimeofday(&starting_time, NULL);
    ageHistory();
    clearKiller(); //clear killer move table
    memset(BestMove, 0, sizeof(BestMove));
    stop_search = false;
    nodes = 0;

    //search
    iterative_deepening(pos, (search_depth == -1)? MAXDEPTH:search_depth, piece_color, op_move);

    //clear tables in analyze mode
    if(analyze)
    {
        clearTT(); //clear main hash table
        clearEvalTT(); //clear evaluation hash table
        clearPawnTT(); //clear pawn hash table
        memset(history, 0, sizeof(history)); //clear history heuristic table
    }
    else
    {
        setAge(); //age tt
    }
}