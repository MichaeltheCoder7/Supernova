#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Board.h"
#include "Transposition.h"

/*
black pieces
00 01 02 03 04 05 06 07
10 11 12 13 14 15 16 17
20 21 22 23 24 25 26 27
30 31 32 33 34 35 36 37
40 41 42 43 44 45 46 47
50 51 52 53 54 55 56 57
60 61 62 63 64 65 66 67
70 71 72 73 74 75 76 77
white pieces
*/

void displayboard(char board[8][8])
{
    int j = 8; /*row numbers*/
    printf("\n    a   b   c   d   e   f   g   h\n");
    /*nested loops to print the board*/
    for(int i = 0; i < 8; i++){
        printf("  +---+---+---+---+---+---+---+---+\n");
        printf(" %d", j);
        for(int k = 0; k < 8; k++){
            if(k == 7){
                printf("| %c |", board[i][k]);
            }
            else{
                printf("| %c ", board[i][k]);
            }
        }
        printf("%d", j);
        printf("\n");
        j--;
    }
    printf("  +---+---+---+---+---+---+---+---+\n");
    printf("    a   b   c   d   e   f   g   h\n\n");
}

//get the x index from a position on the board
int position_to_x(char position[3])
{
    switch(position[1])
    {
        case '8':
            return 0;
        case '7':
            return 1;
        case '6':
            return 2;
        case '5':
            return 3;
        case '4':
            return 4;
        case '3':
            return 5;
        case '2':
            return 6;
        case '1':
            return 7;
    }
    
    return -1; //when nothing is matched
}

//get the y index from a position on the board
int position_to_y(char position[3])
{
    switch(position[0])
    {
        case 'a':
            return 0;
        case 'b':
            return 1;
        case 'c':
            return 2;
        case 'd':
            return 3;
        case 'e':
            return 4;
        case 'f':
            return 5;
        case 'g':
            return 6;
        case 'h':
            return 7;
    }

    return -1; //when nothing is matched
}

//get a position string and translate it to a piece character on the board
char position_to_piece(char board[8][8], char current_position[3])
{
    return board[position_to_x(current_position)][position_to_y(current_position)];
}

//return 1 for capture
//return 2 for promotion
//return 3 for castling
int makeMove(char cur_p[3], char new_p[3], char promotion, BOARD *pos)
{  
    int cur_x = position_to_x(cur_p);
    int cur_y = position_to_y(cur_p);
    int new_x = position_to_x(new_p);
    int new_y = position_to_y(new_p);
    char piece = pos->board[cur_x][cur_y];
    char op_piece = pos->board[new_x][new_y];
    int my_piece = piece_code(piece);
    int cur_64 = 8*cur_x + cur_y;
    int new_64 = 8*new_x + new_y;
    int isCapture = 0;
    int index, square;

    //update board
	pos->board[new_x][new_y] = pos->board[cur_x][cur_y];
	pos->board[cur_x][cur_y] = ' ';

    //castling rights check
    if(pos->board[7][4] != 'K')
    {
        if(pos->ksw)
            pos->key ^= kswcr;
        if(pos->qsw)
            pos->key ^= qswcr;
        pos->ksw = pos->qsw = 0;
    }
    if(pos->board[0][4] != 'k')
    {
        if(pos->ksb)
            pos->key ^= ksbcr;
        if(pos->qsb)
            pos->key ^= qsbcr;
        pos->ksb = pos->qsb = 0;
    }
    if(pos->board[7][0] != 'R')
    {
        if(pos->qsw)
            pos->key ^= qswcr;
        pos->qsw = 0;
    }
    if(pos->board[7][7] != 'R')
    {
        if(pos->ksw)
            pos->key ^= kswcr;
        pos->ksw = 0;
    }
    if(pos->board[0][0] != 'r')
    {
        if(pos->qsb)
            pos->key ^= qsbcr;
        pos->qsb = 0;
    }
    if(pos->board[0][7] != 'r')
    {
        if(pos->ksb)
            pos->key ^= ksbcr;
        pos->ksb = 0;
    }

    //zobrist key update
    pos->key = pos->key ^ table[cur_x][cur_y][my_piece] ^ table[new_x][new_y][my_piece] ^ turn;

    if(op_piece != ' ')//capture
    {
        int enemy_piece = piece_code(op_piece);
        pos->key ^= table[new_x][new_y][enemy_piece];
        pos->piece_num--;
        isCapture = 1;
        //piece list capture
        pos->piece_count[enemy_piece]--;
        index = pos->index_board[new_64];
        square = pos->piece_list[enemy_piece][pos->piece_count[enemy_piece]];
        pos->piece_list[enemy_piece][index] = square;
        pos->index_board[square] = index;
    }

    //piece list update
    index = pos->index_board[cur_64];
    pos->index_board[new_64] = index;
    pos->piece_list[my_piece][index] = new_64;
    
    //fifty moves rule counter update
    if(op_piece == ' ' && toupper(piece) != 'P')
    {
        pos->halfmove_counter++;
    }
    else
    {
        pos->halfmove_counter = 0;
    }

    //encode enpassant
    switch(pos->ep_file)
    {
        case 0:
            break;
        case 1:
            pos->key ^= ep[0];
            break;
        case 2:
            pos->key ^= ep[1];
            break;
        case 3:
            pos->key ^= ep[2];
            break;
        case 4:
            pos->key ^= ep[3];
            break;
        case 5:
            pos->key ^= ep[4];
            break;
        case 6:
            pos->key ^= ep[5];
            break;
        case 7:
            pos->key ^= ep[6];
            break;
        case 8:
            pos->key ^= ep[7];
            break;  
    }

    //update en passant flag
    pos->ep_file = 0;
    if((toupper(piece) == 'P') && abs(cur_x - new_x) == 2)
    {
        switch(new_p[0])
        {
            case 'a':
                pos->key ^= ep[0];
                pos->ep_file = 1;
                break;
            case 'b':
                pos->key ^= ep[1];
                pos->ep_file = 2;
                break;
            case 'c':
                pos->key ^= ep[2];
                pos->ep_file = 3;
                break;
            case 'd':
                pos->key ^= ep[3];
                pos->ep_file = 4;
                break;
            case 'e':
                pos->key ^= ep[4];
                pos->ep_file = 5;
                break;
            case 'f':
                pos->key ^= ep[5];
                pos->ep_file = 6;
                break;
            case 'g':
                pos->key ^= ep[6];
                pos->ep_file = 7;
                break;
            case 'h':
                pos->key ^= ep[7];
                pos->ep_file = 8;
                break;  
        }
    }

    switch(piece)
    {
        case 'P':
            if(new_p[1] == '8')
            {
                //piece list update
                //remove pawn
                pos->piece_count[wP]--;
                index = pos->index_board[new_64];
                square = pos->piece_list[wP][pos->piece_count[wP]];
                pos->piece_list[wP][index] = square;
                pos->index_board[square] = index;
                //hash update
                pos->key ^= table[0][new_y][wP];
                //white Pawn Promotion
                switch(promotion)
                {
                    case 'r':
                        pos->board[0][new_y] = 'R';
                        pos->key ^= table[0][new_y][wR];
                        //add piece
                        pos->piece_list[wR][pos->piece_count[wR]] = new_64;
                        pos->index_board[new_64] = pos->piece_count[wR];
                        pos->piece_count[wR]++;
                        break;
                    case 'b':
                        pos->board[0][new_y] = 'B';
                        pos->key ^= table[0][new_y][wB];
                        //add piece
                        pos->piece_list[wB][pos->piece_count[wB]] = new_64;
                        pos->index_board[new_64] = pos->piece_count[wB];
                        pos->piece_count[wB]++;
                        break;
                    case 'n':
                        pos->board[0][new_y] = 'N';
                        pos->key ^= table[0][new_y][wN];
                        //add piece
                        pos->piece_list[wN][pos->piece_count[wN]] = new_64;
                        pos->index_board[new_64] = pos->piece_count[wN];
                        pos->piece_count[wN]++;
                        break;
                    default:
                        pos->board[0][new_y] = 'Q';
                        pos->key ^= table[0][new_y][wQ];
                        //add piece
                        pos->piece_list[wQ][pos->piece_count[wQ]] = new_64;
                        pos->index_board[new_64] = pos->piece_count[wQ];
                        pos->piece_count[wQ]++;
                        break;
                }
                return 2;
            }
            else if(abs(new_p[0] - cur_p[0]) == 1 && op_piece == ' ')
            {
                //en passant
                pos->board[cur_x][new_y] = ' ';
                pos->key ^= table[cur_x][new_y][bP];
                pos->piece_num--;
                //piece list update
                //remove pawn
                pos->piece_count[bP]--;
                index = pos->index_board[8*cur_x + new_y];
                square = pos->piece_list[bP][pos->piece_count[bP]];
                pos->piece_list[bP][index] = square;
                pos->index_board[square] = index;
                return 1;
            }
            break;
        case 'p':
            if(new_p[1] == '1')
            {
                //piece list update
                //remove pawn
                pos->piece_count[bP]--;
                index = pos->index_board[new_64];
                square = pos->piece_list[bP][pos->piece_count[bP]];
                pos->piece_list[bP][index] = square;
                pos->index_board[square] = index;
                //hash update
                pos->key ^= table[7][new_y][bP];
                //black Pawn Promotion 
                switch(promotion)
                {
                    case 'r':
                        pos->board[7][new_y] = 'r';
                        pos->key ^= table[7][new_y][bR];
                        //add piece
                        pos->piece_list[bR][pos->piece_count[bR]] = new_64;
                        pos->index_board[new_64] = pos->piece_count[bR];
                        pos->piece_count[bR]++;
                        break;
                    case 'b':
                        pos->board[7][new_y] = 'b';
                        pos->key ^= table[7][new_y][bB];
                        //add piece
                        pos->piece_list[bB][pos->piece_count[bB]] = new_64;
                        pos->index_board[new_64] = pos->piece_count[bB];
                        pos->piece_count[bB]++;
                        break;
                    case 'n':
                        pos->board[7][new_y] = 'n';
                        pos->key ^= table[7][new_y][bN];
                        //add piece
                        pos->piece_list[bN][pos->piece_count[bN]] = new_64;
                        pos->index_board[new_64] = pos->piece_count[bN];
                        pos->piece_count[bN]++;
                        break;
                    default:
                        pos->board[7][new_y] = 'q';
                        pos->key ^= table[7][new_y][bQ];
                        //add piece
                        pos->piece_list[bQ][pos->piece_count[bQ]] = new_64;
                        pos->index_board[new_64] = pos->piece_count[bQ];
                        pos->piece_count[bQ]++;
                        break;
                }
                return 2;
            }
            else if(abs(new_p[0] - cur_p[0]) == 1 && op_piece == ' ')
            {   
                //en passant
                pos->board[cur_x][new_y] = ' ';
                pos->key ^= table[cur_x][new_y][wP];
                pos->piece_num--;
                //piece list update
                //remove pawn
                pos->piece_count[wP]--;
                index = pos->index_board[8*cur_x + new_y];
                square = pos->piece_list[wP][pos->piece_count[wP]];
                pos->piece_list[wP][index] = square;
                pos->index_board[square] = index;
                return 1;
            }
            break;
        case 'K':
            //castling
            if(!strncmp(cur_p, "e1", 2) && !strncmp(new_p, "g1", 2))
            {
                pos->board[7][5] = 'R';
                pos->board[7][7] = ' ';
                pos->key ^= table[7][7][3];
                pos->key ^= table[7][5][3];
                //piece list update
                index = pos->index_board[h1];
                pos->index_board[f1] = index;
                pos->piece_list[wR][index] = f1;
                pos->wcastled = true;
                return 3;
            }
            else if(!strncmp(cur_p, "e1", 2) && !strncmp(new_p, "c1", 2))
            {
                pos->board[7][3] = 'R';
                pos->board[7][0] = ' ';
                pos->key ^= table[7][0][3];
                pos->key ^= table[7][3][3];
                //piece list update
                index = pos->index_board[a1];
                pos->index_board[d1] = index;
                pos->piece_list[wR][index] = d1;
                pos->wcastled = true;
                return 3;
            }
            break;
        case 'k':
            //castling
            if(!strncmp(cur_p, "e8", 2) && !strncmp(new_p, "g8", 2))
            {
                pos->board[0][5] = 'r';
                pos->board[0][7] = ' ';
                pos->key ^= table[0][7][9];
                pos->key ^= table[0][5][9];
                //piece list update
                index = pos->index_board[h8];
                pos->index_board[f8] = index;
                pos->piece_list[bR][index] = f8;
                pos->bcastled = true;
                return 3;
            }
            else if(!strncmp(cur_p, "e8", 2) && !strncmp(new_p, "c8", 2))
            {
                pos->board[0][3] = 'r';
                pos->board[0][0] = ' ';
                pos->key ^= table[0][0][9];
                pos->key ^= table[0][3][9];
                //piece list update
                index = pos->index_board[a8];
                pos->index_board[d8] = index;
                pos->piece_list[bR][index] = d8;
                pos->bcastled = true;
                return 3;
            }
            break;
    }
    
    return isCapture;
}

//return 1 if promotion
//for quiescence search
int makeMove_qsearch(char cur_p[3], char new_p[3], char piece, char op_piece, BOARD *pos)
{
    int cur_x = position_to_x(cur_p);
    int cur_y = position_to_y(cur_p);
    int new_x = position_to_x(new_p);
    int new_y = position_to_y(new_p);
    int my_piece = piece_code(piece);
    int cur_64 = 8*cur_x + cur_y;
    int new_64 = 8*new_x + new_y;
    int index, square;

    //update board
	pos->board[new_x][new_y] = pos->board[cur_x][cur_y];
	pos->board[cur_x][cur_y] = ' ';

    //zobrist key update
    pos->key = pos->key ^ table[cur_x][cur_y][my_piece] ^ table[new_x][new_y][my_piece] ^ turn;
    if(op_piece != ' ')//capture
    {
        int enemy_piece = piece_code(op_piece);
        pos->key ^= table[new_x][new_y][enemy_piece];
        pos->piece_num--;
        //piece list capture
        pos->piece_count[enemy_piece]--;
        index = pos->index_board[new_64];
        square = pos->piece_list[enemy_piece][pos->piece_count[enemy_piece]];
        pos->piece_list[enemy_piece][index] = square;
        pos->index_board[square] = index;
    }

    //piece list update
    index = pos->index_board[cur_64];
    pos->index_board[new_64] = index;
    pos->piece_list[my_piece][index] = new_64;

    //encode enpassant
    switch(pos->ep_file)
    {
        case 0:
            break;
        case 1:
            pos->key ^= ep[0];
            break;
        case 2:
            pos->key ^= ep[1];
            break;
        case 3:
            pos->key ^= ep[2];
            break;
        case 4:
            pos->key ^= ep[3];
            break;
        case 5:
            pos->key ^= ep[4];
            break;
        case 6:
            pos->key ^= ep[5];
            break;
        case 7:
            pos->key ^= ep[6];
            break;
        case 8:
            pos->key ^= ep[7];
            break;  
    }

    pos->ep_file = 0;

    switch(piece)
    {
        case 'P':
            if(new_p[1] == '8')
            {
                //piece list update
                //remove pawn
                pos->piece_count[wP]--;
                index = pos->index_board[new_64];
                square = pos->piece_list[wP][pos->piece_count[wP]];
                pos->piece_list[wP][index] = square;
                pos->index_board[square] = index;
                //add piece
                pos->piece_list[wQ][pos->piece_count[wQ]] = new_64;
                pos->index_board[new_64] = pos->piece_count[wQ];
                pos->piece_count[wQ]++;
                //white Pawn Promotion
                pos->board[0][new_y] = 'Q';
                pos->key ^= table[0][new_y][wP];
                pos->key ^= table[0][new_y][wQ];
                return 1;
            }
            else if(abs(new_p[0] - cur_p[0]) == 1 && op_piece == ' ')
            {
                //en passant
                pos->board[cur_x][new_y] = ' ';
                pos->key ^= table[cur_x][new_y][bP];
                pos->piece_num--;
                //piece list update
                //remove pawn
                pos->piece_count[bP]--;
                index = pos->index_board[8*cur_x + new_y];
                square = pos->piece_list[bP][pos->piece_count[bP]];
                pos->piece_list[bP][index] = square;
                pos->index_board[square] = index;
            }
            break;
        case 'p':
            if(new_p[1] == '1')
            {
                //piece list update
                //remove pawn
                pos->piece_count[bP]--;
                index = pos->index_board[new_64];
                square = pos->piece_list[bP][pos->piece_count[bP]];
                pos->piece_list[bP][index] = square;
                pos->index_board[square] = index;
                //add piece
                pos->piece_list[bQ][pos->piece_count[bQ]] = new_64;
                pos->index_board[new_64] = pos->piece_count[bQ];
                pos->piece_count[bQ]++;
                //black Pawn Promotion 
                pos->board[7][new_y] = 'q';
                pos->key ^= table[7][new_y][bP];
                pos->key ^= table[7][new_y][bQ];
                return 1;
            }
            else if(abs(new_p[0] - cur_p[0]) == 1 && op_piece == ' ')
            {   
                //en passant
                pos->board[cur_x][new_y] = ' ';
                pos->key ^= table[cur_x][new_y][wP];
                pos->piece_num--;
                //piece list update
                //remove pawn
                pos->piece_count[wP]--;
                index = pos->index_board[8*cur_x + new_y];
                square = pos->piece_list[wP][pos->piece_count[wP]];
                pos->piece_list[wP][index] = square;
                pos->index_board[square] = index;
            }
            break;
    }

    return 0;
}

//return en passant file
int make_nullmove(BOARD *pos)
{
    int temp_ep = pos->ep_file;
    pos->key ^= turn;

    //encode enpassant
    switch(temp_ep)
    {
        case 0:
            break;
        case 1:
            pos->key ^= ep[0];
            break;
        case 2:
            pos->key ^= ep[1];
            break;
        case 3:
            pos->key ^= ep[2];
            break;
        case 4:
            pos->key ^= ep[3];
            break;
        case 5:
            pos->key ^= ep[4];
            break;
        case 6:
            pos->key ^= ep[5];
            break;
        case 7:
            pos->key ^= ep[6];
            break;
        case 8:
            pos->key ^= ep[7];
            break;  
    }

    pos->ep_file = 0;

    return temp_ep;
}

void undo_nullmove(BOARD *pos, int ep_file)
{
    pos->ep_file = ep_file;
    pos->key ^= turn;

    //encode enpassant
    switch(ep_file)
    {
        case 0:
            break;
        case 1:
            pos->key ^= ep[0];
            break;
        case 2:
            pos->key ^= ep[1];
            break;
        case 3:
            pos->key ^= ep[2];
            break;
        case 4:
            pos->key ^= ep[3];
            break;
        case 5:
            pos->key ^= ep[4];
            break;
        case 6:
            pos->key ^= ep[5];
            break;
        case 7:
            pos->key ^= ep[6];
            break;
        case 8:
            pos->key ^= ep[7];
            break;  
    }
}

//for static exchange evaluation
void makeMove_SEE(char cur_p[3], char new_p[3], char board[8][8])
{
    int cur_x = position_to_x(cur_p);
    int cur_y = position_to_y(cur_p);
    int new_x = position_to_x(new_p);
    int new_y = position_to_y(new_p);
    char piece = board[cur_x][cur_y];

	board[new_x][new_y] = board[cur_x][cur_y];
	board[cur_x][cur_y] = ' ';

    if(piece == 'P' || piece == 'p') 
    {
        switch(new_p[1])
        {
            case '8':
                //white Pawn Promotion 
                board[0][new_y] = 'Q';
                break;
            case '1':
                //black Pawn Promotion 
                board[7][new_y] = 'q';
                break;
            default:
                break;
        }
    }
}