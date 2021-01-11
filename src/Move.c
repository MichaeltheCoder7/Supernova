#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "move.h"
#include "board.h"
#include "transposition.h"

const char positions[64][3] = {

            "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
            "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
            "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
            "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
            "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
            "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
            "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
            "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1"

};

void print_move(MOVE move)
{
    char move_string[6];

    move_to_string(&move, move_string);
    printf("%s\n", move_string);
}

inline void clear_move(MOVE *move)
{
    move->from = NOMOVE;
    move->to = NOMOVE;
    move->promotion = ' ';
    move->move_type = NOMOVE;
}

// return 1 if moves are the same, 0 otherwise
inline int compareMove(MOVE *move1, MOVE *move2)
{
    if (move1->from == move2->from && move1->to == move2->to && move1->promotion == move2->promotion)
    {
        return 1;
    }

    return 0;
}

// convert a move string to move struct
inline MOVE string_to_move(char move[6])
{
    char cp[3];
    char np[3];
    char promotion = ' ';
    MOVE smove;

    clear_move(&smove);
    if (!strncmp(move, "", 5))
        return smove;
    sscanf(move, "%2s%2s%c", cp, np, &promotion);
    smove.from = 8 * position_to_x(cp) + position_to_y(cp);
    smove.to = 8 * position_to_x(np) + position_to_y(np);
    if (promotion == 'q' || promotion == 'r' || promotion == 'b' || promotion == 'n')
    {
        smove.promotion = promotion;
    }
    else
    {
        smove.promotion = ' ';
    }

    return smove;
}

// convert a move struct to move string
inline void move_to_string(MOVE *smove, char move[6])
{
    memset(move, 0, 6);
    if (smove->from == NOMOVE)
        return;
    strncpy(move, positions[smove->from], 3);
    strncat(move, positions[smove->to], 3);
    switch (smove->promotion)
    {
        case 'q':
            move[4] = 'q';
            move[5] = '\0';
            break;
        case 'r':
            move[4] = 'r';
            move[5] = '\0';
            break;
        case 'b':
            move[4] = 'b';
            move[5] = '\0';
            break;
        case 'n':
            move[4] = 'n';
            move[5] = '\0';
            break;
        default:
            move[4] = '\0';
            break;
    }
}

// return 1 for capture
// return 2 for promotion
// return 3 for castling
int makeMove(BOARD *pos, MOVE *move)
{
    int cur_64 = move->from;
    int new_64 = move->to;
    int cur_x = cur_64 / 8;
    int cur_y = cur_64 % 8;
    int new_x = new_64 / 8;
    int new_y = new_64 % 8;
    unsigned char piece = pos->board[cur_x][cur_y];
    unsigned char op_piece = pos->board[new_x][new_y];
    int isCapture = 0;
    int index, square;

    // update previous move
    pos->last_move.piece = piece;
    pos->last_move.to = new_64;

    // update board
    pos->board[new_x][new_y] = piece;
    pos->board[cur_x][cur_y] = __;

    // castling rights check
    if (pos->ksw)
    {
        if (pos->board[7][4] != wK || pos->board[7][7] != wR)
        {
            pos->key ^= kswcr;
            pos->ksw = 0;
        }
    }
    if (pos->qsw)
    {
        if (pos->board[7][4] != wK || pos->board[7][0] != wR)
        {
            pos->key ^= qswcr;
            pos->qsw = 0;
        }
    }
    if (pos->ksb)
    {
        if (pos->board[0][4] != bK || pos->board[0][7] != bR)
        {
            pos->key ^= ksbcr;
            pos->ksb = 0;
        }
    }
    if (pos->qsb)
    {
        if (pos->board[0][4] != bK || pos->board[0][0] != bR)
        {
            pos->key ^= qsbcr;
            pos->qsb = 0;
        }
    }

    // Zobrist key update
    pos->key = pos->key ^ table[cur_x][cur_y][piece] ^ table[new_x][new_y][piece] ^ turn;

    if (op_piece != __) // capture
    {
        pos->key ^= table[new_x][new_y][op_piece];
        pos->piece_num--;
        isCapture = 1;
        // piece list capture
        pos->piece_count[op_piece]--;
        index = pos->index_board[new_64];
        square = pos->piece_list[op_piece][pos->piece_count[op_piece]];
        pos->piece_list[op_piece][index] = square;
        pos->index_board[square] = index;
        // pawn hash update when pawn is captured
        if (op_piece == bP)
        {
            pos->pawn_key ^= table[new_x][new_y][bP];
        }
        else if (op_piece == wP)
        {
            pos->pawn_key ^= table[new_x][new_y][wP];
        }
    }

    // piece list update
    index = pos->index_board[cur_64];
    pos->index_board[new_64] = index;
    pos->piece_list[piece][index] = new_64;

    // fifty moves rule counter update
    if (op_piece == __ && piece != wP && piece != bP)
    {
        pos->halfmove_counter++;
    }
    else
    {
        pos->halfmove_counter = 0;
    }

    // remove en passant file in the key
    if (pos->ep_file != 0)
    {
        pos->key ^= ep[pos->ep_file - 1];
    }

    // update en passant flag and key
    pos->ep_file = 0;
    if ((piece == wP || piece == bP) && abs(cur_x - new_x) == 2)
    {
        pos->key ^= ep[new_y];
        pos->ep_file = new_y + 1;
    }

    pos->pawn_push = false; // detect pawn push to the 7th/2nd rank
    switch (piece)
    {
        case wP:
            // pawn hash update
            pos->pawn_key = pos->pawn_key ^ table[cur_x][cur_y][wP] ^ table[new_x][new_y][wP];
            if (new_x == 0)
            {
                // piece list update
                // remove pawn
                pos->piece_count[wP]--;
                index = pos->index_board[new_64];
                square = pos->piece_list[wP][pos->piece_count[wP]];
                pos->piece_list[wP][index] = square;
                pos->index_board[square] = index;
                // hash update
                pos->key ^= table[0][new_y][wP];
                // pawn hash update
                pos->pawn_key ^= table[0][new_y][wP];
                // white pawn promotion
                switch (move->promotion)
                {
                    case 'r':
                        pos->board[0][new_y] = wR;
                        pos->key ^= table[0][new_y][wR];
                        // add piece
                        pos->piece_list[wR][pos->piece_count[wR]] = new_64;
                        pos->index_board[new_64] = pos->piece_count[wR];
                        pos->piece_count[wR]++;
                        break;
                    case 'b':
                        pos->board[0][new_y] = wB;
                        pos->key ^= table[0][new_y][wB];
                        // add piece
                        pos->piece_list[wB][pos->piece_count[wB]] = new_64;
                        pos->index_board[new_64] = pos->piece_count[wB];
                        pos->piece_count[wB]++;
                        break;
                    case 'n':
                        pos->board[0][new_y] = wN;
                        pos->key ^= table[0][new_y][wN];
                        // add piece
                        pos->piece_list[wN][pos->piece_count[wN]] = new_64;
                        pos->index_board[new_64] = pos->piece_count[wN];
                        pos->piece_count[wN]++;
                        break;
                    default:
                        pos->board[0][new_y] = wQ;
                        pos->key ^= table[0][new_y][wQ];
                        // add piece
                        pos->piece_list[wQ][pos->piece_count[wQ]] = new_64;
                        pos->index_board[new_64] = pos->piece_count[wQ];
                        pos->piece_count[wQ]++;
                        break;
                }
                // update move type
                move->move_type = 2;
                return 2;
            }
            else if (abs(new_y - cur_y) == 1 && op_piece == __)
            {
                // en passant
                pos->board[cur_x][new_y] = __;
                pos->key ^= table[cur_x][new_y][bP];
                pos->pawn_key ^= table[cur_x][new_y][bP];
                pos->piece_num--;
                // piece list update
                // remove pawn
                pos->piece_count[bP]--;
                index = pos->index_board[8 * cur_x + new_y];
                square = pos->piece_list[bP][pos->piece_count[bP]];
                pos->piece_list[bP][index] = square;
                pos->index_board[square] = index;
                // update move type
                move->move_type = 1;
                return 1;
            }
            if (new_x == 1)
                pos->pawn_push = true;
            break;
        case bP:
            // pawn hash update
            pos->pawn_key = pos->pawn_key ^ table[cur_x][cur_y][bP] ^ table[new_x][new_y][bP];
            if (new_x == 7)
            {
                // piece list update
                // remove pawn
                pos->piece_count[bP]--;
                index = pos->index_board[new_64];
                square = pos->piece_list[bP][pos->piece_count[bP]];
                pos->piece_list[bP][index] = square;
                pos->index_board[square] = index;
                // hash update
                pos->key ^= table[7][new_y][bP];
                // pawn hash update
                pos->pawn_key ^= table[7][new_y][bP];
                // black pawn promotion 
                switch (move->promotion)
                {
                    case 'r':
                        pos->board[7][new_y] = bR;
                        pos->key ^= table[7][new_y][bR];
                        // add piece
                        pos->piece_list[bR][pos->piece_count[bR]] = new_64;
                        pos->index_board[new_64] = pos->piece_count[bR];
                        pos->piece_count[bR]++;
                        break;
                    case 'b':
                        pos->board[7][new_y] = bB;
                        pos->key ^= table[7][new_y][bB];
                        // add piece
                        pos->piece_list[bB][pos->piece_count[bB]] = new_64;
                        pos->index_board[new_64] = pos->piece_count[bB];
                        pos->piece_count[bB]++;
                        break;
                    case 'n':
                        pos->board[7][new_y] = bN;
                        pos->key ^= table[7][new_y][bN];
                        // add piece
                        pos->piece_list[bN][pos->piece_count[bN]] = new_64;
                        pos->index_board[new_64] = pos->piece_count[bN];
                        pos->piece_count[bN]++;
                        break;
                    default:
                        pos->board[7][new_y] = bQ;
                        pos->key ^= table[7][new_y][bQ];
                        // add piece
                        pos->piece_list[bQ][pos->piece_count[bQ]] = new_64;
                        pos->index_board[new_64] = pos->piece_count[bQ];
                        pos->piece_count[bQ]++;
                        break;
                }
                // update move type
                move->move_type = 2;
                return 2;
            }
            else if (abs(new_y - cur_y) == 1 && op_piece == __)
            {
                // en passant
                pos->board[cur_x][new_y] = __;
                pos->key ^= table[cur_x][new_y][wP];
                pos->pawn_key ^= table[cur_x][new_y][wP];
                pos->piece_num--;
                // piece list update
                // remove pawn
                pos->piece_count[wP]--;
                index = pos->index_board[8 * cur_x + new_y];
                square = pos->piece_list[wP][pos->piece_count[wP]];
                pos->piece_list[wP][index] = square;
                pos->index_board[square] = index;
                // update move type
                move->move_type = 1;
                return 1;
            }
            if (new_x == 6)
                pos->pawn_push = true;
            break;
        case wK:
            // pawn hash update
            pos->pawn_key = pos->pawn_key ^ table[cur_x][cur_y][wK] ^ table[new_x][new_y][wK];
            // castling
            if (cur_64 == 60 && new_64 == 62)
            {
                pos->board[7][5] = wR;
                pos->board[7][7] = __;
                pos->key ^= table[7][7][3];
                pos->key ^= table[7][5][3];
                // piece list update
                index = pos->index_board[h1];
                pos->index_board[f1] = index;
                pos->piece_list[wR][index] = f1;
                // update move type
                move->move_type = 3;
                return 3;
            }
            else if (cur_64 == 60 && new_64 == 58)
            {
                pos->board[7][3] = wR;
                pos->board[7][0] = __;
                pos->key ^= table[7][0][3];
                pos->key ^= table[7][3][3];
                // piece list update
                index = pos->index_board[a1];
                pos->index_board[d1] = index;
                pos->piece_list[wR][index] = d1;
                // update move type
                move->move_type = 3;
                return 3;
            }
            break;
        case bK:
            // pawn hash update
            pos->pawn_key = pos->pawn_key ^ table[cur_x][cur_y][bK] ^ table[new_x][new_y][bK];
            // castling
            if (cur_64 == 4 && new_64 == 6)
            {
                pos->board[0][5] = bR;
                pos->board[0][7] = __;
                pos->key ^= table[0][7][9];
                pos->key ^= table[0][5][9];
                // piece list update
                index = pos->index_board[h8];
                pos->index_board[f8] = index;
                pos->piece_list[bR][index] = f8;
                // update move type
                move->move_type = 3;
                return 3;
            }
            else if (cur_64 == 4 && new_64 == 2)
            {
                pos->board[0][3] = bR;
                pos->board[0][0] = __;
                pos->key ^= table[0][0][9];
                pos->key ^= table[0][3][9];
                // piece list update
                index = pos->index_board[a8];
                pos->index_board[d8] = index;
                pos->piece_list[bR][index] = d8;
                // update move type
                move->move_type = 3;
                return 3;
            }
            break;
    }

    // update move type
    move->move_type = isCapture;

    return isCapture;
}

// return 1 if promotion
// for quiescence search
int makeMove_qsearch(BOARD *pos, MOVE *move)
{
    int cur_64 = move->from;
    int new_64 = move->to;
    int cur_x = cur_64 / 8;
    int cur_y = cur_64 % 8;
    int new_x = new_64 / 8;
    int new_y = new_64 % 8;
    unsigned char piece = pos->board[cur_x][cur_y];
    unsigned char op_piece = pos->board[new_x][new_y];
    int index, square;

    // update board
    pos->board[new_x][new_y] = piece;
    pos->board[cur_x][cur_y] = __;

    // Zobrist key update
    pos->key = pos->key ^ table[cur_x][cur_y][piece] ^ table[new_x][new_y][piece] ^ turn;
    if (op_piece != __) // capture
    {
        pos->key ^= table[new_x][new_y][op_piece];
        pos->piece_num--;
        // piece list capture
        pos->piece_count[op_piece]--;
        index = pos->index_board[new_64];
        square = pos->piece_list[op_piece][pos->piece_count[op_piece]];
        pos->piece_list[op_piece][index] = square;
        pos->index_board[square] = index;
        // pawn hash update when pawn is captured
        if (op_piece == bP)
        {
            pos->pawn_key ^= table[new_x][new_y][bP];
        }
        else if (op_piece == wP)
        {
            pos->pawn_key ^= table[new_x][new_y][wP];
        }
    }

    // piece list update
    index = pos->index_board[cur_64];
    pos->index_board[new_64] = index;
    pos->piece_list[piece][index] = new_64;
    
    // remove en passant file in the key
    if (pos->ep_file != 0)
    {
        pos->key ^= ep[pos->ep_file - 1];
    }
    pos->ep_file = 0;

    switch (piece)
    {
        case wP:
            // pawn hash update
            pos->pawn_key = pos->pawn_key ^ table[cur_x][cur_y][wP] ^ table[new_x][new_y][wP];
            if (new_x == 0)
            {
                // piece list update
                // remove pawn
                pos->piece_count[wP]--;
                index = pos->index_board[new_64];
                square = pos->piece_list[wP][pos->piece_count[wP]];
                pos->piece_list[wP][index] = square;
                pos->index_board[square] = index;
                // add piece
                pos->piece_list[wQ][pos->piece_count[wQ]] = new_64;
                pos->index_board[new_64] = pos->piece_count[wQ];
                pos->piece_count[wQ]++;
                // white pawn promotion
                pos->board[0][new_y] = wQ;
                pos->key ^= table[0][new_y][wP];
                pos->key ^= table[0][new_y][wQ];
                pos->pawn_key ^= table[0][new_y][wP];
                return 1;
            }
            else if (abs(new_y - cur_y) == 1 && op_piece == __)
            {
                // en passant
                pos->board[cur_x][new_y] = __;
                pos->key ^= table[cur_x][new_y][bP];
                pos->pawn_key ^= table[cur_x][new_y][bP];
                pos->piece_num--;
                // piece list update
                // remove pawn
                pos->piece_count[bP]--;
                index = pos->index_board[8 * cur_x + new_y];
                square = pos->piece_list[bP][pos->piece_count[bP]];
                pos->piece_list[bP][index] = square;
                pos->index_board[square] = index;
            }
            break;
        case bP:
            // pawn hash update
            pos->pawn_key = pos->pawn_key ^ table[cur_x][cur_y][bP] ^ table[new_x][new_y][bP];
            if (new_x == 7)
            {
                // piece list update
                // remove pawn
                pos->piece_count[bP]--;
                index = pos->index_board[new_64];
                square = pos->piece_list[bP][pos->piece_count[bP]];
                pos->piece_list[bP][index] = square;
                pos->index_board[square] = index;
                // add piece
                pos->piece_list[bQ][pos->piece_count[bQ]] = new_64;
                pos->index_board[new_64] = pos->piece_count[bQ];
                pos->piece_count[bQ]++;
                // black pawn promotion 
                pos->board[7][new_y] = bQ;
                pos->key ^= table[7][new_y][bP];
                pos->key ^= table[7][new_y][bQ];
                pos->pawn_key ^= table[7][new_y][bP];
                return 1;
            }
            else if (abs(new_y - cur_y) == 1 && op_piece == __)
            {
                // en passant
                pos->board[cur_x][new_y] = __;
                pos->key ^= table[cur_x][new_y][wP];
                pos->pawn_key ^= table[cur_x][new_y][wP];
                pos->piece_num--;
                // piece list update
                // remove pawn
                pos->piece_count[wP]--;
                index = pos->index_board[8 * cur_x + new_y];
                square = pos->piece_list[wP][pos->piece_count[wP]];
                pos->piece_list[wP][index] = square;
                pos->index_board[square] = index;
            }
            break;
        case wK:
            // pawn hash update
            pos->pawn_key = pos->pawn_key ^ table[cur_x][cur_y][wK] ^ table[new_x][new_y][wK];
            break;
        case bK:
            // pawn hash update
            pos->pawn_key = pos->pawn_key ^ table[cur_x][cur_y][bK] ^ table[new_x][new_y][bK];
            break;
    }

    return 0;
}

// for null move pruning
inline void make_nullmove(BOARD *pos)
{
    pos->key ^= turn;

    // remove en passant file in the key
    if (pos->ep_file != 0)
    {
        pos->key ^= ep[pos->ep_file - 1];
    }
    pos->ep_file = 0;

    // change last move to no move
    pos->last_move.piece = NOMOVE;
}

// for static exchange evaluation
// don't change pawn to queen when promotion occurs for simplicity
inline void makeMove_SEE(unsigned char board[8][8], int cur_x, int cur_y, int new_x, int new_y)
{
    unsigned char piece = board[cur_x][cur_y];
    unsigned char op_piece = board[new_x][new_y];
    board[new_x][new_y] = piece;
    board[cur_x][cur_y] = __;

    if (piece == wP || piece == bP)
    {
        // en passant
        if (op_piece == __)
            board[cur_x][new_y] = __;
    }
}