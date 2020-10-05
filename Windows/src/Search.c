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
#include "MoveGen.h"
#include "OrderMove.h"
#include "Evaluate.h"
#include "Transposition.h"

#define LOWERBOUND  0
#define EXACT       1
#define UPPERBOUND  2
#define ASWINDOW    50
#define INFINITE    20000

//global variables:
int nodes;
struct timeval starting_time;
char BestMove[6];
char searched_move[6];

//age history heuristics array
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

//prevent history heuristics array to overflow 
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
    for(int x = history_index+ply-2; x >= history_index+ply-counter; x-=2)
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

//quiescence search with captures and queen promotion
static int quiescence(BOARD *pos, int color, int alpha, int beta)
{
    int value;
    int length;
    char cp[3];
    char np[3];
    int isprom;
    char moved_piece, piece;
    int cap_piece_value;
    BOARD pos_copy;

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
    
    //delta prunning
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
    char moves[100][6];
    int scores[100];
    length = captureGen(pos, moves, scores, color);

    for(int x = 0; x < length; x++)
    {
        //find the move with highest score
        movesort(moves, scores, length, x);
        
        //make a copy of the board
        pos_copy = *pos;
        sscanf(moves[x], "%2s%2s", cp, np);
        moved_piece = position_to_piece(pos_copy.board, cp);
        piece = position_to_piece(pos_copy.board, np);
        isprom = makeMove_qsearch(cp, np, moved_piece, piece, &pos_copy);
            
        //check if check is ignored
        if(ifCheck(&pos_copy, color))
        {
            continue;
        }

        cap_piece_value = piece_value(piece);

        //delta prunning
        if(!isprom && (standing_pat + cap_piece_value + 200) < alpha)
        {
            if(pos_copy.piece_num > 14)
                continue;
        }

        //SEE prunning
        //only check when higher takes lower and not promotion
        if(!isprom && piece_value(moved_piece) > cap_piece_value)
        {
            if(SEE(pos_copy.board, np, cap_piece_value, color) < 0)
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
                return beta;   //beta cut-off
            } 
            alpha = value;   
        }
    }
    return alpha;
}

//principal variation search
static int pvs(BOARD *pos, int depth, int ply, int color, int alpha, int beta, bool DoNull, bool is_PV)
{   
    int value = -INFINITE;
    int length;
    char cp[3];
    char np[3];
    struct DataItem* entry;
    int entryFlag = UPPERBOUND;
    char bm[6] = "";
    char hash_move[6] = "";
    bool alpha_raised = false;
    int mate_value = INFINITE - ply;
    bool futility = false;
    int moves_made = 0;
    int reduction_depth = 0;
    int new_depth;
    char promotion = ' ';
    int isTactical;
    BOARD pos_copy;
    int extension;
    bool check_extend = false;

    //check if time is up
    timeUp();
    if(stop_search)
        return 0;

    //mate distance prunning
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
        check_extend = true;
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
            //at pv nodes, only exact cutoffs are accepted
            if(is_PV)
            {
                if(entry->flag == EXACT)
                    return entry -> evaluation;
            }
            //tt cutoff at none pv nodes
            else
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
        }
        //get the pv move
        if(entry->flag != UPPERBOUND)
            strncpy(hash_move, entry->bestmove, 6);
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
    
    //static null move prunning
    if(depth <= 2 && !is_PV && !isCheck && abs(beta) < 19000)
    {
        int margin = 120 * depth;
        if(eval - margin >= beta)
        {
            return eval - margin;
        }
    }

    //null move prunning
    //only at non-PV nodes
    if(DoNull && !isCheck && depth >= 3 && !is_PV && eval >= beta)
    {
        if(pos->piece_num > 14)
        {
            int R = 2;
            if(depth > 6)
            {
                R = 3;
            }

            //make null move
            int ep_file = make_nullmove(pos);

            value = -pvs(pos, depth - 1 - R, ply + 1, -color, -beta, -beta + 1, false, false);

            //undo null move
            undo_nullmove(pos, ep_file);
            
            if(stop_search)
                return 0;

            if(value >= beta)
            {
                return beta;
            }
        }
    }
    
    //futility prunning
    int futilityMargin[4] = {0, 200, 300, 500};

    if(depth <= 3 && !is_PV && !isCheck && abs(alpha) < 19000 && (eval + futilityMargin[depth]) <= alpha)
    {
        futility = true;
    }

    //get children of node
    char moves[256][6];
    int scores[256];
    length = moveGen(pos, moves, scores, ply, color);
    int move_exist = orderHashMove(moves, scores, length, hash_move);

    for(int x = 0; x < length; x++)
    {
        //find the move with highest score
        if(x != 0 || !move_exist)
            movesort(moves, scores, length, x);
        if(x == 0)
            strncpy(bm, moves[0], 6);
        //make a copy of the board
        pos_copy = *pos;
        sscanf(moves[x], "%2s%2s%c", cp, np, &promotion);
        isTactical = makeMove(cp, np, promotion, &pos_copy);

        //check if check is ignored
        if(ifCheck(&pos_copy, color))
        {
            continue;
        }

        //fultility prunning
        //try at least one move
        if(futility && !isTactical && moves_made)
        {
            if(!ifCheck(&pos_copy, -color))
            {
                continue;
            }
        }

        moves_made++;

        extension = 0;

        //late move reduction
        reduction_depth = 0;
        new_depth = depth - 1 + extension;
       
        if(!is_PV 
            && new_depth > 3
            && moves_made > 3
            && !isCheck 
            && !isTactical 
            && strncmp(killers[ply][0], moves[x], 5)
            && strncmp(killers[ply][1], moves[x], 5))
        {
            if(!ifCheck(&pos_copy, -color))
            {
                reduction_depth = 1;
                if(moves_made > 6)
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

        //check if lmr failed
        if(reduction_depth && value > alpha)
        {
            new_depth += reduction_depth;
            reduction_depth = 0;
            goto search_again;
        }

        if(stop_search)
            return 0;

        if(value > alpha)
        {
            strncpy(bm, moves[x], 6);
            if(value >= beta)
            {   
                if(!isTactical)
                {
                    //make sure killer moves are different
                    if(strncmp(killers[ply][0], moves[x], 5))
                    {
                        strncpy(killers[ply][1], killers[ply][0], 6);
                    }
                    //save killer move
                    strncpy(killers[ply][0], moves[x], 6);
                    //save data in the history table for move ordering
                    int a = (color==1)?1:0;
                    int b = 8*position_to_x(cp) + position_to_y(cp);
                    int c = 8*position_to_x(np) + position_to_y(np);
                    history[a][b][c] += depth*depth;
                    //prevent overflow 
                    if(history[a][b][c] > 800000)
                    {
                        preventOverflow();
                    }
                }
                alpha = beta;
                entryFlag = LOWERBOUND;
                break;   //beta cut-off
            }
            alpha_raised = true;
            alpha = value;
            entryFlag = EXACT;
        }
    }

    if(!moves_made)
    {
        strncpy(bm, "", 6);
        if(isCheck)
        {
            alpha = -INFINITE + ply; //checkmate
        }
        else
        {
            alpha = 0;  //stalemate
        }
    }
    //draw by fifty moves
    else if(pos->halfmove_counter >= 100)
    {
        return 0;
    }

    //transposition table store:    
    storeTT(pos->key, alpha, eval, depth, bm, entryFlag);
    return alpha; 
}

//principal variation search at root
static int pvs_root(BOARD *pos, int depth, int color, int alpha, int beta)
{   
    int value = -INFINITE;
    int length;
    char cp[3];
    char np[3];
    struct DataItem* entry;
    int entryFlag = UPPERBOUND;
    char bm[6] = "";
    char hash_move[6] = "";
    char promotion = ' ';
    int isTactical;
    BOARD pos_copy;

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
        strncpy(hash_move, entry->bestmove, 6);
    }

    //get children of node
    char moves[256][6];
    int scores[256];
    length = moveGen_root(pos, moves, scores, BestMove, hash_move, color);

    for(int x = 0; x < length; x++)
    {
        //find the move with highest score
        movesort(moves, scores, length, x);
        //make a copy of the board
        pos_copy = *pos;
        sscanf(moves[x], "%2s%2s%c", cp, np, &promotion);
        isTactical = makeMove(cp, np, promotion, &pos_copy);
        
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
            strncpy(bm, moves[x], 6);
            if(value >= beta)
            {   
                if(!isTactical)
                {
                    //make sure killer moves are different
                    if(strncmp(killers[0][0], moves[x], 5))
                    {
                        strncpy(killers[0][1], killers[0][0], 6);
                    }
                    //save killer move 
                    strncpy(killers[0][0], moves[x], 6);
                    //save data in the history table for move ordering
                    int a = (color==1)?1:0;
                    int b = 8*position_to_x(cp) + position_to_y(cp);
                    int c = 8*position_to_x(np) + position_to_y(np);
                    history[a][b][c] += depth*depth;
                    //prevent overflow 
                    if(history[a][b][c] > 800000)
                    {
                        preventOverflow();
                    }
                }
                alpha = beta;
                entryFlag = LOWERBOUND;
                break;   //beta cut-off
            }
            alpha = value;
            entryFlag = EXACT;
        }
    }

    //transposition table store:    
    strncpy(searched_move, bm, 6); //save the best move
    storeTT(pos->key, alpha, VALUENONE, depth, bm, entryFlag);
    return alpha;
}

//obtain the principal variation from the hash table
static inline void getPVline(BOARD *pos, char bestmove[6], int depth)
{
    struct DataItem* item;
    char cp[3];
    char np[3];
    char move[6];
    char promotion = ' ';
    BOARD pos_copy;
    //make a copy
    pos_copy = *pos;  

    strncpy(move, bestmove, 6);
    strncpy(pv_table[0], bestmove, 6);
    
    for(int x = 1; x < depth; x++)
    {   
        if(!strncmp("", move, 5))
        {
            return;
        }
        sscanf(move, "%2s%2s%c", cp, np, &promotion);
        makeMove(cp, np, promotion, &pos_copy);

        item = probeTT(pos_copy.key);
        if(item != NULL)
        {
            strncpy(pv_table[x], item->bestmove, 6);
            strncpy(move, item->bestmove, 6);
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

    for(current_depth = 1; current_depth <= depth; current_depth++)
    {   
        memset(searched_move, 0, sizeof(searched_move));

        val = pvs_root(pos, current_depth, color, alpha, beta);
        
        gettimeofday(&ending_time, NULL);
        secs = (double)(ending_time.tv_usec - starting_time.tv_usec) / 1000000 + (double)(ending_time.tv_sec - starting_time.tv_sec);
        if(stop_search)
        {
            //allow partial search results if at least one move searched and it's within the bounds/not failed low
            if(strncmp(searched_move, "", 5) && !failed_low && val > alpha && val < beta)
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
            if(current_depth >= 6 && more_time && extra_time)
            {
                search_time *= 1.8;
                ponder_time *= 1.8;
                more_time = false;
            }
            failed_low = true;
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
        getPVline(pos, searched_move, current_depth);
        if(current_depth != 1 && strncmp(BestMove, searched_move, 5))
        {
            eazy_move = false;
        }
        strncpy(BestMove, searched_move, 6);

        //send info to gui
        if(val > 19000)
        {
            printf("info depth %d score mate %d nodes %d time %d nps %d pv ", current_depth, (INFINITE - val - 1) / 2 + 1, nodes, (int)(secs*1000), get_nps(nodes, secs));
        }
        else if(val < -19000)
        {
            printf("info depth %d score mate %d nodes %d time %d nps %d pv ", current_depth, -(INFINITE + val - 1) / 2 - 1, nodes, (int)(secs*1000), get_nps(nodes, secs));
        }
        else
        {
            printf("info depth %d score cp %d nodes %d time %d nps %d pv ", current_depth, val, nodes, (int)(secs*1000), get_nps(nodes, secs));
        }

        for(int i = 0; i < current_depth; i++)
        {
            printf("%s ", pv_table[i]);
        }
        printf("\n");

        if(valid_partial_search)
        {
            break;
        }
        //easy move if the best move is a recapture and has not changed at depth 10
        if(eazy_move && current_depth == 10)
        {
            if(isRecapture(BestMove, op_move))
                break;
        }
    }
    //send move to gui
    printf("bestmove %s ponder %s\n", BestMove, pv_table[1]);
}

void search(BOARD *pos, int piece_color, char op_move[6]) 
{
    //prepare to search
    gettimeofday(&starting_time, NULL);
    ageHistory();
    memset(killers, 0, sizeof(killers)); //clear killer move table
    memset(BestMove, 0, sizeof(BestMove));
    stop_search = false;
    nodes = 0;

    //search
    iterative_deepening(pos, (search_depth == -1)? MAXDEPTH:search_depth, piece_color, op_move);

    //clear tables in analyze mode
    if(analyze)
    {
        clearTT();  //clear hash table
        clearEvalTT();  //clear evaluation hash table
        memset(history, 0, sizeof(history)); //clear history heuristic table
    }
    else
    {
        setAge(false); //age tt
    }
}