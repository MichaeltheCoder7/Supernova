#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>
#include "Attack.h"
#include "Search.h"
#include "Board.h"
#include "MoveGeneration.h"
#include "Evaluate.h"


#define EMPTY -1
#define LOWERBOUND 0
#define EXACT 1
#define UPPERBOUND 2
#define ASWINDOW 50 //aspiration window
#define INFINITE 20000

//global variables:
int nodes;
struct timeval starting_time;
char BestMove[6];
char searched_move[6];
bool stop_search;

//count number of pieces on the board
static inline int piece_count(char board[8][8])
{
    int count = 0;
    for(int x = 0; x < 8; x++)
    {
        for(int y = 0; y < 8; y++)
        {
            if(board[x][y] != ' ')
            {
                count++;
            }
        }
    }
    return count;
}

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

//check for time up or stop command from GUI every 512 nodes
static inline void timeUp()
{
    if(!stop_search && !(nodes & 511))
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

int index(char piece) 
{ 
    switch(piece)
    {
        case 'P':
            return 0; 
        case 'N':
            return 1; 
        case 'B':
            return 2; 
        case 'R':
            return 3; 
        case 'Q': 
            return 4; 
        case 'K': 
            return 5; 
        case 'p': 
            return 6; 
        case 'n': 
            return 7; 
        case 'b': 
            return 8; 
        case 'r': 
            return 9; 
        case 'q': 
            return 10; 
        case 'k':
            return 11; 
        default:
            break;
    }
    
    return -1;
} 

//return a 64 bit random number
static unsigned long long llrand() 
{
    unsigned long long r = 0;

    for (int i = 0; i < 5; ++i) {
        r = (r << 15) | (rand() & 0x7FFF);
    }

    return r & 0xFFFFFFFFFFFFFFFFULL;
}

//initialize zobrist hashing:
void init_zobrist()
{
    //fill a table of random numbers
    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            for(int k = 0; k < 12; k++)
            table[i][j][k] = llrand();
        }
    }
    turn = llrand();
    for(int i = 0; i < 8; i++)
    {
        ep[i] = llrand();
    }
    kswcr = llrand();
    qswcr = llrand();
    ksbcr = llrand();
    qsbcr = llrand();
}

//use zobrist hashing to encode chess board
unsigned long long int getHash(char board[8][8], int color, char op_cp[3], char op_np[3], int ksw, int qsw, int ksb, int qsb)
{
    unsigned long long int h = 0;
    int chess_piece;
    for(int x = 0; x < 8; x++)
    {
        for(int y = 0; y < 8; y++)
        {
            if(board[x][y] != ' ')
            {
                chess_piece = index(board[x][y]);
                h ^= table[x][y][chess_piece];
            }
        }
    }
    //encode turn
    if(color == 1)
    {
        h ^= turn;
    }
    //encode castling rights
    if(ksw)
    {
        h ^= kswcr;
    }
    if(qsw)
    {
        h ^= qswcr;
    }
    if(ksb)
    {
        h ^= ksbcr;
    }
    if(qsb)
    {
        h ^= qsbcr;
    }
    //encode enpassant
    if(strncmp("", op_cp, 3) && strncmp("", op_np, 3))
    {
        int steps = abs(op_cp[1] - op_np[1]);
        char piece = position_to_piece(board, op_np);
        if((piece == 'p' || piece == 'P') && steps == 2)
        {
            switch(op_np[0])
            {
                case 'a':
                    h ^= ep[0];
                    break;
                case 'b':
                    h ^= ep[1];
                    break;
                case 'c':
                    h ^= ep[2];
                    break;
                case 'd':
                    h ^= ep[3];
                    break;
                case 'e':
                    h ^= ep[4];
                    break;
                case 'f':
                    h ^= ep[5];
                    break;
                case 'g':
                    h ^= ep[6];
                    break;
                case 'h':
                    h ^= ep[7];
                    break;  
            }
        }
    }
    
    return h;
}

//transposition/hash table:
//+1 index for collision
static inline struct DataItem *probeTT(unsigned long long int key)
{
    //get the hash 
    int hashIndex = key % (HASHSIZE - 1);
    if(tt[hashIndex].flag != EMPTY && tt[hashIndex].key == key)
    {    
        tt[hashIndex].age = false;
        return &tt[hashIndex];
    }
    else if(tt[hashIndex + 1].flag != EMPTY && tt[hashIndex + 1].key == key)
    {    
        tt[hashIndex + 1].age = false;
        return &tt[hashIndex + 1];
    }
    else
    {
        return NULL;
    }

}

static inline void storeTT(unsigned long long int key, int evaluation, int depth, char bestmove[5], int flag)
{
    if(stop_search) //don't save when time up
        return;

    //get the hash 
    int hashIndex = key % (HASHSIZE - 1);

    if(tt[hashIndex].flag == EMPTY || (tt[hashIndex].key == key && tt[hashIndex].depth <= depth) || tt[hashIndex].age == true)
    {
        tt[hashIndex].key = key;
        tt[hashIndex].evaluation = evaluation;
        tt[hashIndex].flag = flag;
        tt[hashIndex].depth = depth;
        strncpy(tt[hashIndex].bestmove, bestmove, 6);
        tt[hashIndex].age = false;
    }
    else if(tt[hashIndex].key != key && (tt[hashIndex + 1].depth <= depth || tt[hashIndex + 1].age == true))
    {
        tt[hashIndex + 1].key = key;
        tt[hashIndex + 1].evaluation = evaluation;
        tt[hashIndex + 1].flag = flag;
        tt[hashIndex + 1].depth = depth;
        strncpy(tt[hashIndex + 1].bestmove, bestmove, 6);
        tt[hashIndex + 1].age = false;
    }
}

//set age in tt
static inline void setAge(bool print_usage)
{
    float count = 0;
    for(int x = 0; x < HASHSIZE; x++)
    {
        if(tt[x].flag != EMPTY)
            count++;

        tt[x].age = true;
    }
    if(print_usage)
        printf("Table Usage: %.2f%%\n", count * 100 / HASHSIZE);
}

//clear the hash table
void clearTT()
{
    for(int x = 0; x < HASHSIZE; x++)
    {
        tt[x].key = 0;
        tt[x].depth = 0;
        tt[x].flag = EMPTY;
        tt[x].evaluation = 0;
        tt[x].age = false;
        strncpy(tt[x].bestmove, "", 6);
    }
}

static inline bool check_repetition(unsigned long long int key, int ply, int counter)
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
static inline int contempt(char board[8][8], int ply)
{
    if(piece_count(board) <= 14)
    {
        return 0;
    }

    if((ply % 2) == 0)
        return -25;

    return 25;
}

//quiescence search with captures and queen promotion
static int quiescence(char board[8][8], int color, int alpha, int beta, char op_cp[3], char op_np[3])
{
    int value;
    int length;
    char board_copy[8][8];
    char cp[3];
    char np[3];
    int isprom;
    char moved_piece, piece;
    int cap_piece_value;

    //check if time is up
    timeUp();
    if(stop_search)
        return 0;

    nodes++;

    int standing_pat = evaluate(board, color);

    if(standing_pat >= beta) 
    {
        return beta;
    }
    
    if(standing_pat > alpha) 
    {
        alpha = standing_pat;
    }
    
    value = -INFINITE;
    //get children of node
    char moves[100][6];
    length = captureGen(board, moves, op_cp, op_np, color);
    cap_ordering(board, moves, length, color);

    for(int x = 0; x < length; x++)
    {
        sscanf(moves[x], "%2s%2s", cp, np);
        //make a copy of the board
        memcpy(board_copy, board, sizeof(board_copy));
        
        moved_piece = position_to_piece(board, cp);
        piece = position_to_piece(board, np);
        isprom = make_move(cp, np, moved_piece, piece, board_copy);
            
        //check if check is ignored
        if(ifCheck(board_copy, color))
        {
            continue;
        }

        cap_piece_value = piece_value(piece);

        //delta prunning
        if((standing_pat + cap_piece_value + 200) < alpha && isprom != 1)
        {
            if(piece_count(board_copy) > 14)
                continue;
        }

        //SEE prunning
        //do not check when captures by pawn or higher takes lower or equal
        if(moved_piece != 'P' && moved_piece != 'p' && cap_piece_value < (piece_value(moved_piece) - 50))
        {
            if((SEE(board_copy, np, color) + cap_piece_value) < 0)
            {
                continue;
            }
        }

        value = -quiescence(board_copy, -color, -beta, -alpha, cp, np);

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
static int pvs(char board[8][8], int depth, int ply, int color, int alpha, int beta, char op_cp[3], char op_np[3], int ksw, int qsw, int ksb, int qsb, int move_counter, bool DoNull, bool is_PV)
{   
    int value = -INFINITE;
    int length;
    char board_copy[8][8];
    char cp[3];
    char np[3];
    struct DataItem* entry;
    int entryFlag = UPPERBOUND;
    char bm[6] = "";
    char pv_move[6] = "";
    int ksb2, qsb2, ksw2, qsw2;
    unsigned long long int key; 
    bool alpha_raised = false;
    int mate_value = INFINITE - ply;
    bool futility = false;
    int moves_made = 0;
    int reduction_depth = 0;
    int new_depth;
    int counter;
    char promotion = ' ';
    char moved_piece;
    char piece;
    int isprom_ep;

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
    
    int isCheck = ifCheck(board, color);
    //check extension
    if(isCheck)
    {
        depth++;
    }

    key = getHash(board, color, op_cp, op_np, ksw, qsw, ksb, qsb);

    //check draw of repetition
    if(check_repetition(key, ply, move_counter))
    {
        nodes++;
        return contempt(board, ply);
    }

    //if depth reaches the end
    if(depth == 0)
    {
       return quiescence(board, color, alpha, beta, op_cp, op_np);
    }
    
    nodes++;

    //transposition table look up
    entry = probeTT(key);
    if(entry != NULL)
    { 
        if(entry -> depth >= depth) 
        {
            //at pv nodes, only exact cutoffs are accepted
            if(is_PV)
            {
                if(entry -> flag == EXACT)
                    return entry -> evaluation;
            }
            //tt cutoff at none pv nodes
            else
            {
                switch(entry -> flag)
                {
                    case EXACT:
                    {   
                        return entry -> evaluation;
                        break;
                    }
                    case LOWERBOUND:
                    {
                        if(beta <= entry -> evaluation)
                            return beta;
                        break;
                    }
                    case UPPERBOUND:
                    {
                        if(alpha >= entry -> evaluation)
                            return alpha;
                        break;
                    }
                }
            }
        }
        //get the pv move if not failed low
        if(entry -> flag != UPPERBOUND)
            strncpy(pv_move, entry->bestmove, 6);
    }

    //get static eval
    int eval = 0;
    if(!is_PV && !isCheck)
        eval = evaluate(board, color);

    //razoring
    if(!is_PV && !isCheck && depth == 1 && eval <= alpha - 300)
    {
        return quiescence(board, color, alpha, beta, op_cp, op_np);
    }
    
    //static null move prunning
    if(depth < 3 && !is_PV && !isCheck && abs(beta) < 19000)
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
        if(piece_count(board) > 14)
        {
            int R = 2;
            if(depth > 6)
            {
                R = 3;
            }

            value = -pvs(board, depth - 1 - R, ply + 1, -color, -beta, -beta + 1, "", "", ksw, qsw, ksb, qsb, move_counter, false, false);
            
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
    length = moveGen(board, moves, op_cp, op_np, ksw, qsw, ksb, qsb, color);
    move_ordering(board, moves, length, pv_move, ply, color);
    strncpy(bm, moves[0], 6);

    for(int x = 0; x < length; x++)
    {
        ksb2 = ksb;
        qsb2 = qsb;
        ksw2 = ksw;
        qsw2 = qsw;
        sscanf(moves[x], "%2s%2s%c", cp, np, &promotion);

        //make a copy of the board
        memcpy(board_copy, board, sizeof(board_copy));
        moved_piece = position_to_piece(board, cp);
        piece = position_to_piece(board, np);
        isprom_ep = makeMove(cp, np, promotion, moved_piece, piece, board_copy);

        //check if check is ignored
        if(ifCheck(board_copy, color))
        {
            continue;
        }

        //fultility prunning
        //try at least one move
        if(futility && piece == ' ' && !isprom_ep && moves_made)
        {
            if(!ifCheck(board_copy, -color))
            {
                continue;
            }
        }

        moves_made++;

        //castling rights check
        if(ksb2 == 1 || qsb2 == 1 || ksw2 == 1 || qsw2 == 1)
        {
            if(board_copy[7][4] != 'K')
            {
                ksw2 = qsw2 = 0;
            }
            if(board_copy[0][4] != 'k')
            {
                ksb2 = qsb2 = 0;
            }
            if(board_copy[7][0] != 'R')
            {
                qsw2 = 0;
            }
            if(board_copy[7][7] != 'R')
            {
                ksw2 = 0;
            }
            if(board_copy[0][0] != 'r')
            {
                qsb2 = 0;
            }
            if(board_copy[0][7] != 'r')
            {
                ksb2 = 0;
            }
        }

        //fifty moves rule
        counter = move_counter;
        if(piece == ' ' && moved_piece != 'P' && moved_piece != 'p')
        {
            counter++;
        }
        else
        {
            counter = 0;
        }
      
        //late move reduction
        reduction_depth = 0;
        new_depth = depth - 1;
       
        if(!is_PV 
            && new_depth > 3
            && moves_made > 3
            && !isCheck 
            && piece == ' ' 
            && !isprom_ep 
            && strncmp(killers[ply][0], moves[x], 5)
            && strncmp(killers[ply][1], moves[x], 5))
        {
            if(!ifCheck(board_copy, -color))
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
            value = -pvs(board_copy, new_depth, ply + 1, -color, -beta, -alpha, cp, np, ksw2, qsw2, ksb2, qsb2, counter, true, is_PV);
        }
        else
        {
            value = -pvs(board_copy, new_depth, ply + 1, -color, -alpha - 1, -alpha, cp, np, ksw2, qsw2, ksb2, qsb2, counter, true, false);
            if(value > alpha)
            {
                value = -pvs(board_copy, new_depth, ply + 1, -color, -beta, -alpha, cp, np, ksw2, qsw2, ksb2, qsb2, counter, true, true);
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
                if(piece == ' ' && !isprom_ep)
                {
                    strncpy(killers[ply][1], killers[ply][0], 6);
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
    else if(move_counter >= 100)
    {
        return 0;
    }

    //transposition table store:    
    storeTT(key, alpha, depth, bm, entryFlag);
    return alpha; 
}

//principal variation search at root
static int pvs_root(char board[8][8], int depth, int color, int alpha, int beta, char op_cp[3], char op_np[3], int ksw, int qsw, int ksb, int qsb, int move_counter)
{   
    int value = -INFINITE;
    int length;
    char board_copy[8][8];
    char cp[3];
    char np[3];
    struct DataItem* entry;
    int entryFlag = UPPERBOUND;
    char bm[6] = "";
    char pv_move[6] = "";
    int ksb2, qsb2, ksw2, qsw2;
    int counter;
    char promotion = ' ';
    char moved_piece;
    char piece;
    int isprom_ep;
    unsigned long long int key;

    int isCheck = ifCheck(board, color);
    //check extension
    if(isCheck)
    {
        depth++;
    }
    
    //transposition table look up
    key = getHash(board, color, op_cp, op_np, ksw, qsw, ksb, qsb);
    entry = probeTT(key);
    if(entry != NULL && entry->flag != UPPERBOUND)
    { 
        strncpy(pv_move, entry->bestmove, 6);
    }
   
    //get children of node
    char moves[256][6];
    length = moveGen(board, moves, op_cp, op_np, ksw, qsw, ksb, qsb, color);
    move_ordering(board, moves, length, pv_move, 0, color);
    
    for(int x = 0; x < length; x++)
    {
        ksb2 = ksb;
        qsb2 = qsb;
        ksw2 = ksw;
        qsw2 = qsw;
        sscanf(moves[x], "%2s%2s%c", cp, np, &promotion);
        //make a copy of the board
        memcpy(board_copy, board, sizeof(board_copy));
        moved_piece = position_to_piece(board, cp);
        piece = position_to_piece(board, np);
        isprom_ep = makeMove(cp, np, promotion, moved_piece, piece, board_copy);
        
        //check if check is ignored
        if(ifCheck(board_copy, color))
        {
            continue;
        }

        //castling rights check
        if(ksb2 == 1 || qsb2 == 1 || ksw2 == 1 || qsw2 == 1)
        {
            if(board_copy[7][4] != 'K')
            {
                ksw2 = qsw2 = 0;
            }
            if(board_copy[0][4] != 'k')
            {
                ksb2 = qsb2 = 0;
            }
            if(board_copy[7][0] != 'R')
            {
                qsw2 = 0;
            }
            if(board_copy[7][7] != 'R')
            {
                ksw2 = 0;
            }
            if(board_copy[0][0] != 'r')
            {
                qsb2 = 0;
            }
            if(board_copy[0][7] != 'r')
            {
                ksb2 = 0;
            }
        }

        //fifty moves rule
        counter = move_counter;
        if(piece == ' ' && moved_piece != 'P' && moved_piece != 'p')
        {
            counter++;
        }
        else
        {
            counter = 0;
        }
        
        if(value == -INFINITE)
        {
            value = -pvs(board_copy, depth - 1, 1, -color, -beta, -alpha, cp, np, ksw2, qsw2, ksb2, qsb2, counter, true, true);
        }
        else
        {
            value = -pvs(board_copy, depth - 1, 1, -color, -alpha - 1, -alpha, cp, np, ksw2, qsw2, ksb2, qsb2, counter, true, false);
            if(value > alpha)
            {
                value = -pvs(board_copy, depth - 1, 1, -color, -beta, -alpha, cp, np, ksw2, qsw2, ksb2, qsb2, counter, true, true);
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
                if(piece == ' ' && !isprom_ep)
                {
                    strncpy(killers[0][1], killers[0][0], 6);
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
    strncpy(searched_move, bm, 6);
    storeTT(key, alpha, depth, bm, entryFlag);
    return alpha;
}

//obtain the principal variation from the hash table
static inline void getPVline(char board[8][8], char bestmove[6], int depth, int ksw, int qsw, int ksb, int qsb, int color)
{
    struct DataItem* item;
    char board_copy[8][8];
    char cp[3];
    char np[3];
    char move[6];
    int turn = color;
    char promotion = ' ';
    memcpy(board_copy, board, sizeof(board_copy));  

    strncpy(move, bestmove, 6);
    strncpy(pv_table[0], bestmove, 6);
    
    for(int x = 1; x < depth; x++)
    {   
        if(!strncmp("", move, 5))
        {
            return;
        }
        sscanf(move, "%2s%2s%c", cp, np, &promotion);
        makeMove(cp, np, promotion, position_to_piece(board_copy, cp), position_to_piece(board_copy, np), board_copy);
        
        if(board_copy[7][4] != 'K')
        {
            ksw = qsw = 0;
        }
        if(board_copy[0][4] != 'k')
        {
            ksb = qsb = 0;
        }
        if(board_copy[7][0] != 'R')
        {
            qsw = 0;
        }
        if(board_copy[7][7] != 'R')
        {
            ksw = 0;
        }
        if(board_copy[0][0] != 'r')
        {
            qsb = 0;
        }
        if(board_copy[0][7] != 'r')
        {
            ksb = 0;
        }

        turn = -turn;
        item = probeTT(getHash(board_copy, turn, cp, np, ksw, qsw, ksb, qsb));
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

//iterative deepening
static void iterative_deepening(char board[8][8], int depth, char op_cp[3], char op_np[3], int ksw, int qsw, int ksb, int qsb, int move_counter, int color)
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

    for(current_depth = 1; current_depth <= depth; current_depth++)
    {   
        memset(searched_move, 0, sizeof(searched_move));

        val = pvs_root(board, current_depth, color, alpha, beta, op_cp, op_np, ksw, qsw, ksb, qsb, move_counter);
        
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
            current_depth -= 1;
            //search longer if failed low
            if(current_depth > 4 && more_time && extra_time)
            {
                search_time *= 1.8;
                ponder_time *= 1.8;
                more_time = false;
            }
            failed_low = true;
            continue;
        }
        else if(val >= beta)
        {
            alpha = -INFINITE;
            beta = INFINITE;
            current_depth -= 1;
            failed_low = false;
            continue;
        }

        failed_low = false;

        alpha = val - ASWINDOW;
        beta = val + ASWINDOW;

        //get the pv line and best move
        memset(pv_table, 0, sizeof(pv_table));
        getPVline(board, searched_move, current_depth, ksw, qsw, ksb, qsb, color);
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
    }
  
    //send move to gui
    printf("bestmove %s ponder %s\n", BestMove, pv_table[1]);
}

void search(char board[8][8], int piece_color, char op_cp[3], char op_np[3], int ksw, int qsw, int ksb, int qsb, int move_counter) 
{
    //prepare to search
    gettimeofday(&starting_time, NULL);
    ageHistory();
    memset(killers, 0, sizeof(killers)); //clear killer move table
    memset(BestMove, 0, sizeof(BestMove));
    nodes = 0;
    stop_search = false;

    //search
    iterative_deepening(board, (search_depth == -1)?MAXDEPTH:search_depth, op_cp, op_np, ksw, qsw, ksb, qsb, move_counter, piece_color);

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
        //EvalTT_usage();
    }
}




