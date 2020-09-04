#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>

char board[8][8] = {
						
                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                    {' ', 'r', ' ', ' ', ' ', ' ', ' ', ' '},			
                    {'p', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                    {' ', 'P', ' ', 'Q', ' ', ' ', ' ', ' '},
                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
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

static inline void updateboard(char current_position[3], char new_position[3], char board[8][8])
{
    /*finding the location of the current position*/
    int current_x = position_to_x(current_position);
    int current_y = position_to_y(current_position);
    /*make the new position equal to the current position, then make current position blank*/
    board[position_to_x(new_position)][position_to_y(new_position)] = board[current_x][current_y]; /*new is now the current*/
    board[current_x][current_y] = ' ';
	
}

//get a position string and translate it to a piece character on the board
char position_to_piece(char board[8][8], char current_position[3])
{
    return board[position_to_x(current_position)][position_to_y(current_position)];
}

//return 1 if promotion
//return 2 for enpassant
//for quiescence search and static exchange evaluation
int make_move(char cur_p[3], char new_p[3], char board[8][8])
{
    char piece = position_to_piece(board, cur_p);
    char ep = position_to_piece(board, new_p);

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

static inline int cap_piece_value(char board[8][8], char np[3])
{
    switch(position_to_piece(board, np))
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
        default:
            return 100;
    }
}

int get_smallest_attacker(char board[8][8], char location[3], int color)
{
    int x = position_to_x(location);
    int y = position_to_y(location);
    int attackers_index[2] = {-2, -2};

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

static inline int piece_value(char piece)
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
        default:
            return 100;
    }
}

//static exchange evaluation for quiescence search
int SEE(char board[8][8], char location[3], int color)
{
    char board_copy[8][8];
    int value = 0;
    int x;
    int y;
    int attacker_index = get_smallest_attacker(board, location, color);
    char piece;

    if(attacker_index != -1)
    {
        piece = position_to_piece(board, location);
        //stop when king is captured
        if(piece == 'K' || piece == 'k')
        {
            return -20000;
        }
        
        x = attacker_index / 8;
        y = attacker_index % 8;
        memcpy(board_copy, board, sizeof(board_copy));
        make_move(positions[x][y], location, board_copy);
        value = -piece_value(piece) - SEE(board_copy, location, -color);
    }
    return value;
}

//static exchange evaluation for quiescence search
int SEE2(char board[8][8], char location[3], int color)
{
    char board_copy[8][8];
    int value = 0;
    int x;
    int y;
    int attacker_index = get_smallest_attacker(board, location, color);
    char piece;
    int turn = 1;
    bool king_attack = false;
    memcpy(board_copy, board, sizeof(board_copy));

    while(attacker_index != -1)
    {
        piece = position_to_piece(board_copy, location);
        x = attacker_index / 8;
        y = attacker_index % 8;
        
        if(board_copy[x][y] == 'K' || board_copy[x][y] == 'k')
            king_attack = true;            
       
        make_move(positions[x][y], location, board_copy);
        color = -color;
        attacker_index = get_smallest_attacker(board_copy, location, color);
        
        //stop when king is captured
        if(king_attack && attacker_index != -1)
            break;
        
        value -= turn * piece_value(piece);
        turn = -turn;
    }
    return value;
}

int main()
{
    char location[3] = "b4";
    int value = SEE2(board, location, -1);
    printf("value is %d\n", value);

    return 0;
}