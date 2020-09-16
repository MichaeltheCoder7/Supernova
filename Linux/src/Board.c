#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Board.h"


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

static inline void updateboard(char current_position[3], char new_position[3], char board[8][8])
{
    /*finding the location of the current position*/
    int current_x = position_to_x(current_position);
    int current_y = position_to_y(current_position);
    /*make the new position equal to the current position, then make current position blank*/
    board[position_to_x(new_position)][position_to_y(new_position)] = board[current_x][current_y]; /*new is now the current*/
    board[current_x][current_y] = ' ';
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

//return 1 if promotion
//return 2 for enpassant
//for quiescence search and static exchange evaluation
int make_move(char cur_p[3], char new_p[3], char piece, char ep, char board[8][8])
{
    updateboard(cur_p, new_p, board);

    // CPU Pawn Promotion
    if(piece == 'P' || piece == 'p') 
    {
        switch(new_p[1])
        {
            // White Pawn Promotion 
            case '8':
            {
                board[0][position_to_y(new_p)] = 'Q';
                return 1;
                break;
            }
            case '1':
            {
                //Black Pawn Promotion 
                board[7][position_to_y(new_p)] = 'q';
                return 1;
                break;
            }
            default:
            {
                break;
            }
        }

        //en passant
        if(abs(new_p[0] - cur_p[0]) == 1 && ep == ' ')
        {
            board[position_to_x(cur_p)][position_to_y(new_p)] = ' ';
            return 2;
        }
    }

    return 0;
}

//return 1 if promotion
//return 2 for enpassant
//have underpromotions
int makeMove(char cur_p[3], char new_p[3], char promotion, char piece, char ep, char board[8][8])
{
    updateboard(cur_p, new_p, board);

    if(piece == 'P' || piece == 'p') 
    {
        switch(new_p[1])
        {
            // White Pawn Promotion 
            case '8':
            {
                switch(promotion)
                {
                    case 'r':
                        board[0][position_to_y(new_p)] = 'R';
                        break;
                    case 'b':
                        board[0][position_to_y(new_p)] = 'B';
                        break;
                    case 'n':
                        board[0][position_to_y(new_p)] = 'N';
                        break;
                    default:
                        board[0][position_to_y(new_p)] = 'Q';
                        break;
                }
                return 1;
                break;
            }
            case '1':
            {
                //Black Pawn Promotion 
                switch(promotion)
                {
                    case 'r':
                        board[7][position_to_y(new_p)] = 'r';
                        break;
                    case 'b':
                        board[7][position_to_y(new_p)] = 'b';
                        break;
                    case 'n':
                        board[7][position_to_y(new_p)] = 'n';
                        break;
                    default:
                        board[7][position_to_y(new_p)] = 'q';
                        break;
                }
                return 1;
                break;
            }
            default:
            {
                break;
            }
        }

        //en passant
        if(abs(new_p[0] - cur_p[0]) == 1 && ep == ' ')
        {
            board[position_to_x(cur_p)][position_to_y(new_p)] = ' ';
            return 2;
        }
    }
    //For castling:
    else if(piece == 'K' || piece == 'k') 
    {
        if(strncmp(cur_p, "e1", 2) == 0 && strncmp(new_p, "g1", 2) == 0)
        {
            updateboard("h1", "f1", board);
        }
        else if(strncmp(cur_p, "e8", 2) == 0 && strncmp(new_p, "g8", 2) == 0)
        {
            updateboard("h8", "f8", board);
        }
        else if(strncmp(cur_p, "e1", 2) == 0 && strncmp(new_p, "c1", 2) == 0)
        {
            updateboard("a1", "d1", board);
        }
        else if(strncmp(cur_p, "e8", 2) == 0 && strncmp(new_p, "c8", 2) == 0)
        {
            updateboard("a8", "d8", board);
        }
    }

    return 0;
}


//reset the board to the starting positions
void resetboard(char board[8][8])
{
    char board_reset[8][8] = {
                        
                            {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
                            {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
                            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},			
                            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                            {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
                            {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'},

                            };

    memcpy(board, board_reset, sizeof(board_reset));
}