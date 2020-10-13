#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>

char board[8][8] = {
						
                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                    {' ', ' ', ' ', ' ', ' ', 'k', ' ', ' '},			
                    {' ', ' ', ' ', ' ', 'p', ' ', ' ', ' '},
                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                    {' ', ' ', ' ', 'N', ' ', ' ', ' ', ' '},
                    {' ', ' ', ' ', ' ', 'R', ' ', ' ', ' '},
                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},

                    };

char positions[8][8][3] = {

                            {"a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"},
                            {"a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7"},
                            {"a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6"},
                            {"a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5"},
                            {"a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4"},
                            {"a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3"},
                            {"a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2"},
                            {"a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1"},

                            };

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

//for static exchange evaluation
void makeMove_SEE(char board[8][8], int cur_x, int cur_y, int new_x, int new_y)
{
    char piece = board[cur_x][cur_y];
    char op_piece = board[new_x][new_y];
	board[new_x][new_y] = piece;
	board[cur_x][cur_y] = ' ';

    if(piece == 'P' || piece == 'p') 
    {
        switch(new_x)
        {
            case '0':
                //white Pawn Promotion 
                board[0][new_y] = 'Q';
                break;
            case '7':
                //black Pawn Promotion 
                board[7][new_y] = 'q';
                break;
            default:
                break;
        }
        //en passant
        if(op_piece == ' ')
            board[cur_x][new_y] = ' ';
    }
}

int get_smallest_attacker(char board[8][8], int x, int y, int color)
{
    int attackers_index[2] = {-2, -2}; //for queen and king

    switch(color)
    {
        case 1:
        {
            //pawn attack
            if(x + 1 <= 7 && y - 1 >= 0)
            {
                if(board[x + 1][y - 1] == 'P')
                {
                    return 8*(x+1) + (y-1);
                }
            }
            if(x + 1 <= 7 && y + 1 <= 7)
            {
                if(board[x + 1][y + 1] == 'P')
                {
                    return 8*(x+1) + (y+1);
                }
            }
            //knight attacks
            if(x + 2 <= 7 && y + 1 <= 7)
            {
                if(board[x + 2][y + 1] == 'N')
                    return 8*(x+2) + (y+1);
            }
            if(x + 2 <= 7 && y - 1 >= 0)
            {
                if(board[x + 2][y - 1] == 'N')
                    return 8*(x+2) + (y-1);
            }
            if(x + 1 <= 7 && y + 2 <= 7)
            {
                if(board[x + 1][y + 2] == 'N')
                    return 8*(x+1) + (y+2);
            }
            if(x + 1 <= 7 && y - 2 >= 0)
            {
                if(board[x + 1][y - 2] == 'N')
                    return 8*(x+1) + (y-2);
            }
            if(x - 2 >= 0 && y + 1 <= 7)
            {
                if(board[x - 2][y + 1] == 'N')
                    return 8*(x-2) + (y+1);
            }
            if(x - 2 >= 0 && y - 1 >= 0)
            {
                if(board[x - 2][y - 1] == 'N')
                    return 8*(x-2) + (y-1);
            }
            if(x - 1 >= 0 && y + 2 <= 7)
            {
                if(board[x - 1][y + 2] == 'N')
                    return 8*(x-1) + (y+2);
            }
            if(x - 1 >= 0 && y - 2 >= 0)
            {
                if(board[x - 1][y - 2] == 'N')
                    return 8*(x-1) + (y-2);
            }
            //down left
            for(int i = 1; i <= 7; i++)
            {
                if(x + i > 7 || y - i < 0)
                {
                    break;
                }
                if(board[x + i][y - i] != ' ')
                {
                    if(board[x + i][y - i] == 'B')
                    {
                        return 8*(x+i) + (y-i);
                    }
                    else if(board[x + i][y - i] == 'Q')
                    {
                        attackers_index[0] = 8*(x+i) + (y-i);
                    }
                    else if(i == 1 && board[x + i][y - i] == 'K')
                    {
                        attackers_index[1] = 8*(x+i) + (y-i);
                    }
                    break;
                }
            }
            //down right
            for(int i = 1; i <= 7; i++)
            {
                if(x + i > 7 || y + i > 7)
                {
                    break;
                }
                if(board[x + i][y + i] != ' ')
                {
                    if(board[x + i][y + i] == 'B')
                    {
                        return 8*(x+i) + (y+i);
                    }
                    else if(board[x + i][y + i] == 'Q')
                    {
                        attackers_index[0] = 8*(x+i) + (y+i);
                    }
                    else if(i == 1 && board[x + i][y + i] == 'K')
                    {
                        attackers_index[1] = 8*(x+i) + (y+i);
                    }
                    break;
                }
            }
            //up left
            for(int i = 1; i <= 7; i++)
            {
                if(x - i < 0 || y - i < 0)
                {
                    break;
                }
                if(board[x - i][y - i] != ' ')
                {
                    if(board[x - i][y - i] == 'B')
                    {
                        return 8*(x-i) + (y-i);
                    }
                    else if(board[x - i][y - i] == 'Q')
                    {
                        attackers_index[0] = 8*(x-i) + (y-i);
                    }
                    else if(i == 1 && board[x - i][y - i] == 'K')
                    {
                        attackers_index[1] = 8*(x-i) + (y-i);
                    }
                    break;
                }
            }
            //up right
            for(int i = 1; i <= 7; i++)
            {
                if(x - i < 0 || y + i > 7)
                {
                    break;
                }
                if(board[x - i][y + i] != ' ')
                {
                    if(board[x - i][y + i] == 'B')
                    {
                        return 8*(x-i) + (y+i);
                    }
                    else if(board[x - i][y + i] == 'Q')
                    {
                        attackers_index[0] = 8*(x-i) + (y+i);
                    }
                    else if(i == 1 && board[x - i][y + i] == 'K')
                    {
                        attackers_index[1] = 8*(x-i) + (y+i);
                    }
                    break;
                }
            }
            //down
            for(int i = x+1; i <= 7; i++)
            {
                if(board[i][y] != ' ')
                {
                    if(board[i][y] == 'R')
                    {
                        return 8*i + y;
                    }
                    else if(board[i][y] == 'Q')
                    {
                        attackers_index[0] = 8*i + y;
                    }
                    else if(i == x+1 && board[i][y] == 'K')
                    {
                        attackers_index[1] = 8*i + y;
                    }
                    break;
                }
            }
            //left
            for(int i = y-1; i >= 0; i--)
            {
                if(board[x][i] != ' ')
                {
                    if(board[x][i] == 'R')
                    {
                        return 8*x + i;
                    }
                    else if(board[x][i] == 'Q')
                    {
                        attackers_index[0] = 8*x + i;
                    }
                    else if(i == y-1 && board[x][i] == 'K')
                    {
                        attackers_index[1] = 8*x + i;
                    }

                    break;
                }
            }
            //right
            for(int i = y+1; i <= 7; i++)
            {
                if(board[x][i] != ' ')
                {
                    if(board[x][i] == 'R')
                    {
                        return 8*x + i;
                    }
                    else if(board[x][i] == 'Q')
                    {
                        attackers_index[0] = 8*x + i;
                    }
                    else if(i == y+1 && board[x][i] == 'K')
                    {
                        attackers_index[1] = 8*x + i;
                    }
                    break;
                }
            }
            //up
            for(int i = x-1; i >= 0; i--)
            {
                if(board[i][y] != ' ')
                {
                    if(board[i][y] == 'R')
                    {
                        return 8*i + y;
                    }
                    else if(board[i][y] == 'Q')
                    {
                        attackers_index[0] = 8*i + y;
                    }
                    else if(i == x-1 && board[i][y] == 'K')
                    {
                        attackers_index[1] = 8*i + y;
                    }
                    break;
                }
            }

            break;
        }
        
        case -1:
        {
            //pawn attack
            if(x - 1 >= 0 && y - 1 >= 0)
            {
                if(board[x - 1][y - 1] == 'p')
                {
                    return 8*(x-1) + (y-1);
                }
            }
            if(x - 1 >= 0 && y + 1 <= 7)
            {
                if(board[x - 1][y + 1] == 'p')
                {
                    return 8*(x-1) + (y+1);
                }
            }
            //knight attacks
            if(x - 2 >= 0 && y + 1 <= 7)
            {
                if(board[x - 2][y + 1] == 'n')
                    return 8*(x-2) + (y+1);
            }
            if(x - 2 >= 0 && y - 1 >= 0)
            {
                if(board[x - 2][y - 1] == 'n')
                    return 8*(x-2) + (y-1);
            }
            if(x - 1 >= 0 && y + 2 <= 7)
            {
                if(board[x - 1][y + 2] == 'n')
                    return 8*(x-1) + (y+2);
            }
            if(x - 1 >= 0 && y - 2 >= 0)
            {
                if(board[x - 1][y - 2] == 'n')
                    return 8*(x-1) + (y-2);
            }
            if(x + 2 <= 7 && y + 1 <= 7)
            {
                if(board[x + 2][y + 1] == 'n')
                    return 8*(x+2) + (y+1);
            }
            if(x + 2 <= 7 && y - 1 >= 0)
            {
                if(board[x + 2][y - 1] == 'n')
                    return 8*(x+2) + (y-1);
            }
            if(x + 1 <= 7 && y + 2 <= 7)
            {
                if(board[x + 1][y + 2] == 'n')
                    return 8*(x+1) + (y+2);
            }
            if(x + 1 <= 7 && y - 2 >= 0)
            {
                if(board[x + 1][y - 2] == 'n')
                    return 8*(x+1) + (y-2);
            }
            //up left
            for(int i = 1; i <= 7; i++)
            {
                if(x - i < 0 || y - i < 0)
                {
                    break;
                }
                if(board[x - i][y - i] != ' ')
                {
                    if(board[x - i][y - i] == 'b')
                    {
                        return 8*(x-i) + (y-i);
                    }
                    else if(board[x - i][y - i] == 'q')
                    {
                        attackers_index[0] = 8*(x-i) + (y-i);
                    }
                    else if(i == 1 && board[x - i][y - i] == 'k')
                    {
                        attackers_index[1] = 8*(x-i) + (y-i);
                    }
                    break;
                }
            }
            //up right
            for(int i = 1; i <= 7; i++)
            {
                if(x - i < 0 || y + i > 7)
                {
                    break;
                }
                if(board[x - i][y + i] != ' ')
                {
                    if(board[x - i][y + i] == 'b')
                    {
                        return 8*(x-i) + (y+i);
                    }
                    else if(board[x - i][y + i] == 'q')
                    {
                        attackers_index[0] = 8*(x-i) + (y+i);
                    }
                    else if(i == 1 && board[x - i][y + i] == 'k')
                    {
                        attackers_index[1] = 8*(x-i) + (y+i);
                    }
                    break;
                }
            }
            //down left
            for(int i = 1; i <= 7; i++)
            {
                if(x + i > 7 || y - i < 0)
                {
                    break;
                }
                if(board[x + i][y - i] != ' ')
                {
                    if(board[x + i][y - i] == 'b')
                    {
                        return 8*(x+i) + (y-i);
                    }
                    else if(board[x + i][y - i] == 'q')
                    {
                        attackers_index[0] = 8*(x+i) + (y-i);
                    }
                    else if(i == 1 && board[x + i][y - i] == 'k')
                    {
                        attackers_index[1] = 8*(x+i) + (y-i);
                    }
                    break;
                }
            }
            //down right
            for(int i = 1; i <= 7; i++)
            {
                if(x + i > 7 || y + i > 7)
                {
                    break;
                }
                if(board[x + i][y + i] != ' ')
                {
                    if(board[x + i][y + i] == 'b')
                    {
                        return 8*(x+i) + (y+i);
                    }
                    else if(board[x + i][y + i] == 'q')
                    {
                        attackers_index[0] = 8*(x+i) + (y+i);
                    }
                    else if(i == 1 && board[x + i][y + i] == 'k')
                    {
                        attackers_index[1] = 8*(x+i) + (y+i);
                    }
                    break;
                }
            }
            //up
            for(int i = x-1; i >= 0; i--)
            {
                if(board[i][y] != ' ')
                {
                    if(board[i][y] == 'r')
                    {
                        return 8*i + y;
                    }
                    else if(board[i][y] == 'q')
                    {
                        attackers_index[0] = 8*i + y;
                    }
                    else if(i == x-1 && board[i][y] == 'k')
                    {
                        attackers_index[1] = 8*i + y;
                    }
                    break;
                }
            }
            //left
            for(int i = y-1; i >= 0; i--)
            {
                if(board[x][i] != ' ')
                {
                    if(board[x][i] == 'r')
                    {
                        return 8*x + i;
                    }
                    else if(board[x][i] == 'q')
                    {
                        attackers_index[0] = 8*x + i;
                    }
                    else if(i == y-1 && board[x][i] == 'k')
                    {
                        attackers_index[1] = 8*x + i;
                    }

                    break;
                }
            }
            //right
            for(int i = y+1; i <= 7; i++)
            {
                if(board[x][i] != ' ')
                {
                    if(board[x][i] == 'r')
                    {
                        return 8*x + i;
                    }
                    else if(board[x][i] == 'q')
                    {
                        attackers_index[0] = 8*x + i;
                    }
                    else if(i == y+1 && board[x][i] == 'k')
                    {
                        attackers_index[1] = 8*x + i;
                    }
                    break;
                }
            }
            //down
            for(int i = x+1; i <= 7; i++)
            {
                if(board[i][y] != ' ')
                {
                    if(board[i][y] == 'r')
                    {
                        return 8*i + y;
                    }
                    else if(board[i][y] == 'q')
                    {
                        attackers_index[0] = 8*i + y;
                    }
                    else if(i == x+1 && board[i][y] == 'k')
                    {
                        attackers_index[1] = 8*i + y;
                    }
                    break;
                }
            }

            break;
        }
    }
    
    if(attackers_index[0] != -2)
    {
        return attackers_index[0];
    }
    else if(attackers_index[1] != -2)
    {
        return attackers_index[1];
    }

    return -1;
}

int piece_value(char piece)
{
    switch(piece)
    {
        case 'P':
            return 100;
        case 'p':
            return 100;
        case 'N':
            return 320;
        case 'n':
            return 320;
        case 'B':
            return 330;
        case 'b':
            return 330;
        case 'R':
            return 500;
        case 'r':
            return 500;
        case 'Q':
            return 900;
        case 'q':
            return 900;
        case 'K':
            return 20000;
        case 'k':
            return 20000;
        case ' ':
            return 100;
        default:
            return 0;
    }
}
/*
//static exchange evaluation for quiescence search
int SEE(char board[8][8], char location[3], int target, int color)
{
    int new_x = position_to_x(location);
    int new_y = position_to_y(location);
    int attacker_index = get_smallest_attacker(board, new_x, new_y, color);
    if(attacker_index == -1)
        return target;
    char board_copy[8][8];
    int x, y;
    char piece;
    bool king_attack = false;
    int gain[32];
    int d = 0;
    gain[d] = target;
    memcpy(board_copy, board, sizeof(board_copy));

    do
    {
        d++;
        x = attacker_index / 8;
        y = attacker_index % 8;
        
        if(toupper(board_copy[x][y]) == 'K')
            king_attack = true;            

        piece = position_to_piece(board_copy, location);

        makeMove_SEE(board_copy, x, y, new_x, new_y);
        color = -color;
        attacker_index = get_smallest_attacker(board_copy, new_x, new_y, color);
        
        //stop when king is captured
        if(king_attack && attacker_index != -1)
        {
            gain[d] = -gain[d-1];
            printf("gain[%d]: %d\n", d, gain[d]);
            break;
        }
        gain[d] = piece_value(piece) - gain[d-1];
        printf("gain[%d]: %d\n", d, gain[d]);
        if(((-gain[d-1] >= gain[d])? -gain[d-1]:gain[d]) < 0)
        {
            gain[d] = -gain[d-1];
            break;
        }
    }
    while(attacker_index != -1);

    printf("\n");
    while(--d)
    {
        gain[d] = -((-gain[d] >= gain[d+1])? -gain[d]:gain[d+1]);
        printf("gain[%d]: %d\n", d, gain[d]);
    }
    gain[0] = -((-gain[0] >= gain[1])? -gain[0]:gain[1]);

    return gain[0];
}

//static exchange evaluation for quiescence search
int SEE_MO(char board[8][8], int att_x, int att_y, char location[3], int target, int color)
{
    char board_copy[8][8];
    int x = att_x;
    int y = att_y;
    int attacker_index;
    bool king_attack = false;
    int gain[32];
    int d = 0;
    int new_x = position_to_x(location);
    int new_y = position_to_y(location);
    gain[d] = target;
    memcpy(board_copy, board, sizeof(board_copy));

    do
    {
        d++;
        gain[d] = piece_value(board_copy[x][y]) - gain[d-1];
        printf("gain[%d]: %d\n", d, gain[d]);
        if(toupper(board_copy[x][y]) == 'K')
            king_attack = true;  

        //prunning
        if(((-gain[d-1] >= gain[d])? -gain[d-1]:gain[d]) < 0)
        {
            if(!king_attack)
                break;   
        }

        makeMove_SEE(board_copy, x, y, new_x, new_y);
        attacker_index = get_smallest_attacker(board_copy, new_x, new_y, color);
        color = -color;
        //stop when king is captured
        if(king_attack && attacker_index != -1)
        {
            d--;
            if(d == 0)
                return -20000;
            break;
        }
        
        x = attacker_index / 8;
        y = attacker_index % 8; 
    }
    while(attacker_index != -1);

    printf("\n");
    while(--d)
    {
        gain[d-1] = -((-gain[d-1] >= gain[d])? -gain[d-1]:gain[d]);
        printf("gain[%d]: %d\n", d-1, gain[d-1]);
    }

    return gain[0];
}
*/
//static exchange evaluation for quiescence search
int SEE(char board[8][8], int new_x, int new_y, int target, int color)
{
    int attacker_index = get_smallest_attacker(board, new_x, new_y, color);
    //exit if no attackers found
    if(attacker_index == -1)
        return target;
    char board_copy[8][8];
    int x, y;
    char piece;
    bool king_attack = false;
    int gain[32];
    int d = 0;
    gain[d] = target;
    memcpy(board_copy, board, sizeof(board_copy));

    do
    {
        d++;
        //convert index to 2D
        x = attacker_index / 8;
        y = attacker_index % 8;
        
        if(toupper(board_copy[x][y]) == 'K')
            king_attack = true;            

        piece = board_copy[new_x][new_y];

        makeMove_SEE(board_copy, x, y, new_x, new_y);
        color = -color;
        attacker_index = get_smallest_attacker(board_copy, new_x, new_y, color);
        
        //stop when king is captured
        if(king_attack && attacker_index != -1)
        {
            gain[d] = -gain[d-1];
            printf("gain[%d]: %d\n", d, gain[d]);
            break;
        }

        gain[d] = piece_value(piece) - gain[d-1];
        printf("gain[%d]: %d\n", d, gain[d]);
        //prunning that won't change the result
        if(((-gain[d-1] >= gain[d])? -gain[d-1]:gain[d]) < 0)
        {
            gain[d] = -gain[d-1];
            break;
        }
    }
    while(attacker_index != -1);
    printf("\n");
    while(--d)
    {
        gain[d] = -((-gain[d] >= gain[d+1])? -gain[d]:gain[d+1]);
        printf("gain[%d]: %d\n", d, gain[d]);
    }
    gain[0] = -((-gain[0] >= gain[1])? -gain[0]:gain[1]);

    return gain[0];
}

//static exchange evaluation for move ordering
int SEE_MO(char board[8][8], int att_x, int att_y, int new_x, int new_y, int target, int color)
{
    char board_copy[8][8];
    int x = att_x;
    int y = att_y;
    int attacker_index;
    bool king_attack = false;
    int gain[32];
    int d = 0;
    gain[d] = target;
    memcpy(board_copy, board, sizeof(board_copy));

    do
    {
        d++;
        gain[d] = piece_value(board_copy[x][y]) - gain[d-1];
        printf("gain[%d]: %d\n", d, gain[d]);
        if(toupper(board_copy[x][y]) == 'K')
            king_attack = true; 
        //prunning
        if(((-gain[d-1] >= gain[d])? -gain[d-1]:gain[d]) < 0)
        {
            if(!king_attack)
                break;
        }      

        makeMove_SEE(board_copy, x, y, new_x, new_y);
        //displayboard(board_copy);
        attacker_index = get_smallest_attacker(board_copy, new_x, new_y, color);
        color = -color;
        //stop when king is captured
        if(king_attack && attacker_index != -1)
        {
            d--;
            if(d == 0)
                return -20000;
            break;
        }
        
        x = attacker_index / 8;
        y = attacker_index % 8;
    }
    while(attacker_index != -1);
    printf("\n");
    while(--d)
    {
        gain[d-1] = -((-gain[d-1] >= gain[d])? -gain[d-1]:gain[d]);
        printf("gain[%d]: %d\n", d-1, gain[d-1]);
    }

    return gain[0];
}

int main()
{
    char curloc[3] = "d3";
    int cx = position_to_x(curloc);
    int cy = position_to_y(curloc);
    char location[3] = "e5";
    int x = position_to_x(location);
    int y = position_to_y(location);
    //int value = SEE(board, x, y, 100, -1);
    int value = SEE_MO(board, cx, cy, x, y, 100, -1);
    printf("value is %d\n", value);

    return 0;
}