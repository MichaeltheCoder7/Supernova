#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>

//position 1
//depth 5: 4865609 nodes
//depth 6: 119060324 nodes
char board[8][8] = {
						
							{'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
							{'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
							{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},			
							{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
							{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
							{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
							{'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
							{'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'},

						};

//position 2
//depth 4: 4085603 nodes
//depth 5: 193690690 nodes
char board2[8][8] = {
						
							{'r', ' ', ' ', ' ', 'k', ' ', ' ', 'r'},
							{'p', ' ', 'p', 'p', 'q', 'p', 'b', ' '},
							{'b', 'n', ' ', ' ', 'p', 'n', 'p', ' '},			
							{' ', ' ', ' ', 'P', 'N', ' ', ' ', ' '},
							{' ', 'p', ' ', ' ', 'P', ' ', ' ', ' '},
							{' ', ' ', 'N', ' ', ' ', 'Q', ' ', 'p'},
							{'P', 'P', 'P', 'B', 'B', 'P', 'P', 'P'},
							{'R', ' ', ' ', ' ', 'K', ' ', ' ', 'R'},

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


int knight_moves_x[8] = {-2, -2, -1, -1,  1,  1,  2,  2};
int knight_moves_y[8] = {-1,  1, -2,  2, -2,  2, -1,  1};

int king_moves_x[10] = {-1, -1, -1,  0,  0,  0,  0,  1,  1,  1};
int king_moves_y[10] = {-1,  0,  1, -2, -1,  1,  2, -1,  0,  1};

void displayboard(char board[8][8])
{
	int j = 8; /*row numbers*/
	printf("\n    a   b   c   d   e   f   g   h\n");
	/*nested loops so I can print the board*/
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
        {
            return 0;
        }
        case '7':
        {
            return 1;
        }
        case '6':
        {
            return 2;
        }
        case '5':
        {
            return 3;
        }
        case '4':
        {
            return 4;
        }
        case '3':
        {
            return 5;
        }
        case '2':
        {
            return 6;
        }
        case '1':
        {
            return 7;
        }
    }
    return -1; //when nothing is matched
}

//get the y index from a position on the board
int position_to_y(char position[3])
{
    switch(position[0])
    {
        case 'a':
        {
            return 0;
        }
        case 'b':
        {
            return 1;
        }
        case 'c':
        {
            return 2;
        }
        case 'd':
        {
            return 3;
        }
        case 'e':
        {
            return 4;
        }
        case 'f':
        {
            return 5;
        }
        case 'g':
        {
            return 6;
        }
        case 'h':
        {
            return 7;
        }
    }

    return -1; //when nothing is matched
}

//get a position string and translate it to a piece character on the board
char position_to_piece(char board[8][8], char current_position[3])
{
    return board[position_to_x(current_position)][position_to_y(current_position)];
}

/*displays the board as the players make moves*/
static void updateboard(char current_position[3], char new_position[3], char board[8][8])
{
    /*finding the location of the current position*/
	int current_x = position_to_x(current_position);
	int current_y = position_to_y(current_position);
	/*make the new position equal to the current position, then make current position blank*/
	board[position_to_x(new_position)][position_to_y(new_position)] = board[current_x][current_y]; /*new is now the current*/
	board[current_x][current_y] = ' ';
	
}

//return 1 if promotion
//return 2 for enpassant
//have under promotions
int makeMove(char cur_p[3], char new_p[3], char promotion, char piece, char ep, char board[8][8])
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

void undoMove(char cur_p[3], char new_p[3], int type, char piece, int color, char board[8][8])
{
    /*finding the location of the current position*/
	int new_x = position_to_x(new_p);
	int new_y = position_to_y(new_p);
	/*make the new position equal to the current position, then make current position blank*/
	board[position_to_x(cur_p)][position_to_y(cur_p)] = board[new_x][new_y]; /*new is now the current*/
	board[new_x][new_y] = piece;
    
    switch(type)
    {
        case 1:
            if(color == 1)
            {
                board[position_to_x(cur_p)][position_to_y(cur_p)] = 'p';
            }
            else
            {
                board[position_to_x(cur_p)][position_to_y(cur_p)] = 'P';
            }
            break;
        case 2:
            if(color == 1)
            {
                board[new_x-1][new_y] = 'P';
            }
            else
            {
                board[new_x+1][new_y] = 'p';
            }
            break;
        case 3:
            updateboard("f1", "h1", board);
            break;
        case 4:
            updateboard("f8", "h8", board);
            break;
        case 5:
            updateboard("d1", "a1", board);
            break;
        case 6:
            updateboard("d8", "a8", board);
            break;
        default:
            break;
    }
}

//save one side's all positions in an array of string
//need caller to declare: char opponent[16][3] array first and pass it in
//the array has a fixed size
//color = 1: white pieces
//color = 2: black pieces
static int all_positions(char board[8][8], char opponent[16][3], int color)
{
    int temp = 0;
    //get all white pieces locations
    switch(color)
    {
        case -1:
        {
            for(int x = 0; x < 8; x++)
            {
                for (int y = 0; y < 8; y++)
                {
                    if(isupper(board[x][y]))
                    {
                        strncpy(opponent[temp], positions[x][y], 3);
                        temp++;
                        
                    }
                }
            }
            break;
        }
        //get all black pieces locations
        case 1:
        {
            for(int x = 0; x < 8; x++)
            {
                for (int y = 0; y < 8; y++)
                {
                    if(islower(board[x][y]))
                    {
                        strncpy(opponent[temp], positions[x][y], 3);
                        temp++;
                        
                    }

                }
            }
            break;
        }
    }
    return temp;
}

int ifCheck(char board[8][8], int color)
{
    int king_x = -1;
    int king_y = -1;

    switch(color)
    {
        case 1:
        {
            for(int x = 0; x < 8; x++)
            {
                for (int y = 7; y >= 0; y--)
                {
                    if(board[x][y] == 'k')
                    {
                        king_x = x;
                        king_y = y;
                        x = 8; //to break out of nested for loops
                        break;
                    }

                }
            }
            //down left
            for(int i = 1; i <= 7; i++)
            {
                if(king_x + i > 7 || king_y - i < 0)
                {
                    break;
                }
                if(board[king_x + i][king_y - i] != ' ')
                {
                    if(i == 1 && (board[king_x + i][king_y - i] == 'P' || board[king_x + i][king_y - i] == 'K'))
                    {
                        return 1;
                    }
                    if(board[king_x + i][king_y - i] == 'B' || board[king_x + i][king_y - i] == 'Q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //down right
            for(int i = 1; i <= 7; i++)
            {
                if(king_x + i > 7 || king_y + i > 7)
                {
                    break;
                }
                if(board[king_x + i][king_y + i] != ' ')
                {
                    if(i == 1 && (board[king_x + i][king_y + i] == 'P' || board[king_x + i][king_y + i] == 'K'))
                    {
                        return 1;
                    }
                    if(board[king_x + i][king_y + i] == 'B' || board[king_x + i][king_y + i] == 'Q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //down
            for(int i = king_x+1; i <= 7; i++)
            {
                if(board[i][king_y] != ' ')
                {
                    if(i == king_x+1 && board[i][king_y] == 'K')
                    {
                        return 1;
                    }
                    if(board[i][king_y] == 'R' || board[i][king_y] == 'Q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //left
            for(int i = king_y-1; i >= 0; i--)
            {
                if(board[king_x][i] != ' ')
                {
                    if(i == king_y-1 && board[king_x][i] == 'K')
                    {
                        return 1;
                    }
                    if(board[king_x][i] == 'R' || board[king_x][i] == 'Q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //right
            for(int i = king_y+1; i <= 7; i++)
            {
                if(board[king_x][i] != ' ')
                {
                    if(i == king_y+1 && board[king_x][i] == 'K')
                    {
                        return 1;
                    }
                    if(board[king_x][i] == 'R' || board[king_x][i] == 'Q')
                    {
                        return 1;
                    }
                    break;
                }
            }     
            //knight attacks
            if(king_x - 2 >= 0 && king_y + 1 <= 7)
            {
                if(board[king_x - 2][king_y + 1] == 'N')
                    return 1;
            }
            if(king_x - 2 >= 0 && king_y - 1 >= 0)
            {
                if(board[king_x - 2][king_y - 1] == 'N')
                    return 1;
            }
            if(king_x - 1 >= 0 && king_y + 2 <= 7)
            {
                if(board[king_x - 1][king_y + 2] == 'N')
                    return 1;
            }
            if(king_x - 1 >= 0 && king_y - 2 >= 0)
            {
                if(board[king_x - 1][king_y - 2] == 'N')
                    return 1;
            }
            if(king_x + 2 <= 7 && king_y + 1 <= 7)
            {
                if(board[king_x + 2][king_y + 1] == 'N')
                    return 1;
            }
            if(king_x + 2 <= 7 && king_y - 1 >= 0)
            {
                if(board[king_x + 2][king_y - 1] == 'N')
                    return 1;
            }
            if(king_x + 1 <= 7 && king_y + 2 <= 7)
            {
                if(board[king_x + 1][king_y + 2] == 'N')
                    return 1;
            }
            if(king_x + 1 <= 7 && king_y - 2 >= 0)
            {
                if(board[king_x + 1][king_y - 2] == 'N')
                    return 1;
            }
            //up
            for(int i = king_x-1; i >= 0; i--)
            {
                if(board[i][king_y] != ' ')
                {
                    if(i == king_x-1 && board[i][king_y] == 'K')
                    {
                        return 1;
                    }
                    if(board[i][king_y] == 'R' || board[i][king_y] == 'Q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //up left
            for(int i = 1; i <= 7; i++)
            {
                if(king_x - i < 0 || king_y - i < 0)
                {
                    break;
                }
                if(board[king_x - i][king_y - i] != ' ')
                {
                    if(i == 1 && board[king_x - i][king_y - i] == 'K')
                    {
                        return 1;
                    }
                    if(board[king_x - i][king_y - i] == 'B' || board[king_x - i][king_y - i] == 'Q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //up right
            for(int i = 1; i <= 7; i++)
            {
                if(king_x - i < 0 || king_y + i > 7)
                {
                    break;
                }
                if(board[king_x - i][king_y + i] != ' ')
                {
                    if(i == 1 && board[king_x - i][king_y + i] == 'K')
                    {
                        return 1;
                    }
                    if(board[king_x - i][king_y + i] == 'B' || board[king_x - i][king_y + i] == 'Q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            
            break;
        }
        case -1:
        {
            for(int x = 7; x >= 0; x--)
            {
                for (int y = 7; y >= 0; y--)
                {
                    if(board[x][y] == 'K')
                    {
                        king_x = x;
                        king_y = y;
                        x = -1; //to break out of nested for loops
                        break;
                    }

                }
            }
            //up left
            for(int i = 1; i <= 7; i++)
            {
                if(king_x - i < 0 || king_y - i < 0)
                {
                    break;
                }
                if(board[king_x - i][king_y - i] != ' ')
                {
                    if(i == 1 && (board[king_x - i][king_y - i] == 'p' || board[king_x - i][king_y - i] == 'k'))
                    {
                        return 1;
                    }
                    if(board[king_x - i][king_y - i] == 'b' || board[king_x - i][king_y - i] == 'q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //up right
            for(int i = 1; i <= 7; i++)
            {
                if(king_x - i < 0 || king_y + i > 7)
                {
                    break;
                }
                if(board[king_x - i][king_y + i] != ' ')
                {
                    if(i == 1 && (board[king_x - i][king_y + i] == 'p' || board[king_x - i][king_y + i] == 'k'))
                    {
                        return 1;
                    }
                    if(board[king_x - i][king_y + i] == 'b' || board[king_x - i][king_y + i] == 'q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //up
            for(int i = king_x-1; i >= 0; i--)
            {
                if(board[i][king_y] != ' ')
                {
                    if(i == king_x-1 && board[i][king_y] == 'k')
                    {
                        return 1;
                    }
                    if(board[i][king_y] == 'r' || board[i][king_y] == 'q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //left
            for(int i = king_y-1; i >= 0; i--)
            {
                if(board[king_x][i] != ' ')
                {
                    if(i == king_y-1 && board[king_x][i] == 'k')
                    {
                        return 1;
                    }
                    if(board[king_x][i] == 'r' || board[king_x][i] == 'q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //right
            for(int i = king_y+1; i <= 7; i++)
            {
                if(board[king_x][i] != ' ')
                {
                    if(i == king_y+1 && board[king_x][i] == 'k')
                    {
                        return 1;
                    }
                    if(board[king_x][i] == 'r' || board[king_x][i] == 'q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //knight attacks
            if(king_x - 2 >= 0 && king_y + 1 <= 7)
            {
                if(board[king_x - 2][king_y + 1] == 'n')
                    return 1;
            }
            if(king_x - 2 >= 0 && king_y - 1 >= 0)
            {
                if(board[king_x - 2][king_y - 1] == 'n')
                    return 1;
            }
            if(king_x - 1 >= 0 && king_y + 2 <= 7)
            {
                if(board[king_x - 1][king_y + 2] == 'n')
                    return 1;
            }
            if(king_x - 1 >= 0 && king_y - 2 >= 0)
            {
                if(board[king_x - 1][king_y - 2] == 'n')
                    return 1;
            }
            if(king_x + 2 <= 7 && king_y + 1 <= 7)
            {
                if(board[king_x + 2][king_y + 1] == 'n')
                    return 1;
            }
            if(king_x + 2 <= 7 && king_y - 1 >= 0)
            {
                if(board[king_x + 2][king_y - 1] == 'n')
                    return 1;
            }
            if(king_x + 1 <= 7 && king_y + 2 <= 7)
            {
                if(board[king_x + 1][king_y + 2] == 'n')
                    return 1;
            }
            if(king_x + 1 <= 7 && king_y - 2 >= 0)
            {
                if(board[king_x + 1][king_y - 2] == 'n')
                    return 1;
            }
            //down
            for(int i = king_x+1; i <= 7; i++)
            {
                if(board[i][king_y] != ' ')
                {
                    if(i == king_x+1 && board[i][king_y] == 'k')
                    {
                        return 1;
                    }
                    if(board[i][king_y] == 'r' || board[i][king_y] == 'q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //down left
            for(int i = 1; i <= 7; i++)
            {
                if(king_x + i > 7 || king_y - i < 0)
                {
                    break;
                }
                if(board[king_x + i][king_y - i] != ' ')
                {
                    if(i == 1 && board[king_x + i][king_y - i] == 'k')
                    {
                        return 1;
                    }
                    if(board[king_x + i][king_y - i] == 'b' || board[king_x + i][king_y - i] == 'q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //down right
            for(int i = 1; i <= 7; i++)
            {
                if(king_x + i > 7 || king_y + i > 7)
                {
                    break;
                }
                if(board[king_x + i][king_y + i] != ' ')
                {
                    if(i == 1 && board[king_x + i][king_y + i] == 'k')
                    {
                        return 1;
                    }
                    if(board[king_x + i][king_y + i] == 'b' || board[king_x + i][king_y + i] == 'q')
                    {
                        return 1;
                    }
                    break;
                }
            }

            break;
        }
    }
    return 0;
}

int isThreatened(char board[8][8], char location[3], int color)
{
    int x = position_to_x(location);
    int y = position_to_y(location);

    switch(color)
    {
        case 1:
        {
            //left
            for(int i = y-1; i >= 0; i--)
            {
                if(board[x][i] != ' ')
                {
                    if(i == y-1 && board[x][i] == 'K')
                    {
                        return 1;
                    }
                    if(board[x][i] == 'R' || board[x][i] == 'Q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //right
            for(int i = y+1; i <= 7; i++)
            {
                if(board[x][i] != ' ')
                {
                    if(i == y+1 && board[x][i] == 'K')
                    {
                        return 1;
                    }
                    if(board[x][i] == 'R' || board[x][i] == 'Q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //up
            for(int i = x-1; i >= 0; i--)
            {
                if(board[i][y] != ' ')
                {
                    if(i == x-1 && board[i][y] == 'K')
                    {
                        return 1;
                    }
                    if(board[i][y] == 'R' || board[i][y] == 'Q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //down
            for(int i = x+1; i <= 7; i++)
            {
                if(board[i][y] != ' ')
                {
                    if(i == x+1 && board[i][y] == 'K')
                    {
                        return 1;
                    }
                    if(board[i][y] == 'R' || board[i][y] == 'Q')
                    {
                        return 1;
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
                    if(i == 1 && (board[x + i][y - i] == 'P' || board[x + i][y - i] == 'K'))
                    {
                        return 1;
                    }
                    if(board[x + i][y - i] == 'B' || board[x + i][y - i] == 'Q')
                    {
                        return 1;
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
                    if(i == 1 && (board[x + i][y + i] == 'P' || board[x + i][y + i] == 'K'))
                    {
                        return 1;
                    }
                    if(board[x + i][y + i] == 'B' || board[x + i][y + i] == 'Q')
                    {
                        return 1;
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
                    if(i == 1 && board[x - i][y - i] == 'K')
                    {
                        return 1;
                    }
                    if(board[x - i][y - i] == 'B' || board[x - i][y - i] == 'Q')
                    {
                        return 1;
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
                    if(i == 1 && board[x - i][y + i] == 'K')
                    {
                        return 1;
                    }
                    if(board[x - i][y + i] == 'B' || board[x - i][y + i] == 'Q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //knight attacks
            if(x - 2 >= 0 && y + 1 <= 7)
            {
                if(board[x - 2][y + 1] == 'N')
                    return 1;
            }
            if(x - 2 >= 0 && y - 1 >= 0)
            {
                if(board[x - 2][y - 1] == 'N')
                    return 1;
            }
            if(x - 1 >= 0 && y + 2 <= 7)
            {
                if(board[x - 1][y + 2] == 'N')
                    return 1;
            }
            if(x - 1 >= 0 && y - 2 >= 0)
            {
                if(board[x - 1][y - 2] == 'N')
                    return 1;
            }
            if(x + 2 <= 7 && y + 1 <= 7)
            {
                if(board[x + 2][y + 1] == 'N')
                    return 1;
            }
            if(x + 2 <= 7 && y - 1 >= 0)
            {
                if(board[x + 2][y - 1] == 'N')
                    return 1;
            }
            if(x + 1 <= 7 && y + 2 <= 7)
            {
                if(board[x + 1][y + 2] == 'N')
                    return 1;
            }
            if(x + 1 <= 7 && y - 2 >= 0)
            {
                if(board[x + 1][y - 2] == 'N')
                    return 1;
            }
            break;
        }
        
        case -1:
        {
            //left
            for(int i = y-1; i >= 0; i--)
            {
                if(board[x][i] != ' ')
                {
                    if(i == y-1 && board[x][i] == 'k')
                    {
                        return 1;
                    }
                    if(board[x][i] == 'r' || board[x][i] == 'q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //right
            for(int i = y+1; i <= 7; i++)
            {
                if(board[x][i] != ' ')
                {
                    if(i == y+1 && board[x][i] == 'k')
                    {
                        return 1;
                    }
                    if(board[x][i] == 'r' || board[x][i] == 'q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //up
            for(int i = x-1; i >= 0; i--)
            {
                if(board[i][y] != ' ')
                {
                    if(i == x-1 && board[i][y] == 'k')
                    {
                        return 1;
                    }
                    if(board[i][y] == 'r' || board[i][y] == 'q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //down
            for(int i = x+1; i <= 7; i++)
            {
                if(board[i][y] != ' ')
                {
                    if(i == x+1 && board[i][y] == 'k')
                    {
                        return 1;
                    }
                    if(board[i][y] == 'r' || board[i][y] == 'q')
                    {
                        return 1;
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
                    if(i == 1 && board[x + i][y - i] == 'k')
                    {
                        return 1;
                    }
                    if(board[x + i][y - i] == 'b' || board[x + i][y - i] == 'q')
                    {
                        return 1;
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
                    if(i == 1 && board[x + i][y + i] == 'k')
                    {
                        return 1;
                    }
                    if(board[x + i][y + i] == 'b' || board[x + i][y + i] == 'q')
                    {
                        return 1;
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
                    if(i == 1 && (board[x - i][y - i] == 'p' || board[x - i][y - i] == 'k'))
                    {
                        return 1;
                    }
                    if(board[x - i][y - i] == 'b' || board[x - i][y - i] == 'q')
                    {
                        return 1;
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
                    if(i == 1 && (board[x - i][y + i] == 'p' || board[x - i][y + i] == 'k'))
                    {
                        return 1;
                    }
                    if(board[x - i][y + i] == 'b' || board[x - i][y + i] == 'q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //knight attacks
            if(x - 2 >= 0 && y + 1 <= 7)
            {
                if(board[x - 2][y + 1] == 'n')
                    return 1;
            }
            if(x - 2 >= 0 && y - 1 >= 0)
            {
                if(board[x - 2][y - 1] == 'n')
                    return 1;
            }
            if(x - 1 >= 0 && y + 2 <= 7)
            {
                if(board[x - 1][y + 2] == 'n')
                    return 1;
            }
            if(x - 1 >= 0 && y - 2 >= 0)
            {
                if(board[x - 1][y - 2] == 'n')
                    return 1;
            }
            if(x + 2 <= 7 && y + 1 <= 7)
            {
                if(board[x + 2][y + 1] == 'n')
                    return 1;
            }
            if(x + 2 <= 7 && y - 1 >= 0)
            {
                if(board[x + 2][y - 1] == 'n')
                    return 1;
            }
            if(x + 1 <= 7 && y + 2 <= 7)
            {
                if(board[x + 1][y + 2] == 'n')
                    return 1;
            }
            if(x + 1 <= 7 && y - 2 >= 0)
            {
                if(board[x + 1][y - 2] == 'n')
                    return 1;
            }
            break;
        }
    }
    
    return 0;
}


//checking moves for white pawn
int CheckMove_wpawn(char board[8][8], int index_x, int index_y, int new_x, int new_y, char op_cp[3], char op_np[3])
{
    /* 1 for legal move, 0 for illegal move*/
	/*White Pawn*/
    /*Capturing*/
    if(index_y != new_y)
    {
        if(board[new_x][new_y] != ' ')
        {
            if(board[new_x][new_y] == 'p' || board[new_x][new_y] == 'r' || board[new_x][new_y] == 'n' || board[new_x][new_y] == 'b' || board[new_x][new_y] == 'q')
            {
                return 1;
            }
        }
        else
        {
            //en passant
            if(op_cp[1] == '7' && op_np[1] == '5' && position_to_piece(board, op_np) == 'p')
            {
                if(position_to_y(op_np) == new_y && index_x == 3 && new_x == 2)
                {
                    return 1;
                }                                           
            }
        }
    }
    /*one step forward*/
    else
    {
        if(board[new_x][new_y] == ' ')
        {
            return 1;
        }
    }  
    return 0; 
}                                                     

//checking moves for black pawn
int CheckMove_bpawn(char board[8][8], int index_x, int index_y, int new_x, int new_y, char op_cp[3], char op_np[3])
{
    /* 1 for legal move, 0 for illegal move*/
	/* Black Pawn */
    /*Capturing*/
    if(index_y != new_y)
    {
        if(board[new_x][new_y] != ' ')
        {
            if(board[new_x][new_y] == 'P' || board[new_x][new_y] == 'R' || board[new_x][new_y] == 'N' || board[new_x][new_y] == 'B' || board[new_x][new_y] == 'Q')
            {
                return 1;
            }
        }
        else
        {
            //en passant
            if(op_cp[1] == '2' && op_np[1] == '4' && position_to_piece(board, op_np) == 'P')
            {
                if(position_to_y(op_np) == new_y && index_x == 4 && new_x == 5)
                {
                    return 1;
                }                                           
            }
                                
        }	
    }
    /*one step forward*/
    else
    {
        if(board[new_x][new_y] == ' ')
        {
            return 1;
        }
    }  
    return 0; 
} 

//checking moves for white king
int CheckMove_wking(char board[8][8], int index_x, int index_y, int new_x, int new_y, int ksw, int qsw)
{
    /* 1 for legal move, 0 for illegal move*/
	/*White King*/
    if(abs(new_y - index_y) <= 1)
    {
        if(board[new_x][new_y] == ' ' || board[new_x][new_y] == 'p' || board[new_x][new_y] == 'r' || board[new_x][new_y] == 'n' || board[new_x][new_y] == 'b' || board[new_x][new_y] == 'q')
        {
            return 1;
        }
    }
    //castling
    //king side
    else if(index_y == 4 && index_x == 7 && new_y == 6 && new_x == 7)
    {
        //check if king and rook have been moved
        if(ksw)
        {
            //check if positions between king and rook are empty
            if(board[7][5] == ' ' && board[7][6] == ' ')
            {
                //check if king is in check and the two other spots are threatened
                if((ifCheck(board, -1)) == 0 && (isThreatened(board, "f1", -1)) == 0 && (isThreatened(board, "g1", -1)) == 0)
                {
                    return 1;
                }
            }   
        }   
    }
    //queen side
    else if(index_y == 4 && index_x == 7 && new_y == 2 && new_x == 7)
    {
        //check if king and rook have been moved
        if(qsw)
        {
            //check if positions between king and rook are empty
            if(board[7][1] == ' ' && board[7][2] == ' ' && board[7][3] == ' ')
            {
                //check if king is in check and the two other spots are threatened
                if((ifCheck(board, -1)) == 0 && (isThreatened(board, "c1", -1)) == 0 && (isThreatened(board, "d1", -1)) == 0)
                {
                    return 1;      
                }
            } 
        }
    }
    //end of castling  
    return 0; 
}

//checking moves for black king
int CheckMove_bking(char board[8][8], int index_x, int index_y, int new_x, int new_y, int ksb, int qsb)
{
    /* 1 for legal move, 0 for illegal move*/
	/*Black King*/
    if(abs(new_y - index_y) <= 1)
    {
        if(board[new_x][new_y] == ' ' || board[new_x][new_y] == 'P' || board[new_x][new_y] == 'R' || board[new_x][new_y] == 'N' || board[new_x][new_y] == 'B' || board[new_x][new_y] == 'Q')
        {
            return 1;
        }
    }
    //castling
    //king side
    else if(index_y == 4 && index_x == 0 && new_y == 6 && new_x == 0)
    {
        //check if king and rook have been moved
        if(ksb)
        {
            //check if positions between king and rook are empty
            if(board[0][5] == ' ' && board[0][6] == ' ')
            {
                //check if king is in check and the two other spots are threatened
                if((ifCheck(board, 1)) == 0 && (isThreatened(board, "f8", 1)) == 0 && (isThreatened(board, "g8", 1)) == 0)
                {
                    return 1;
                }
            }          
        }
    }
    //queen side
    else if(index_y == 4 && index_x == 0 && new_y == 2 && new_x == 0)
    {
        //check if king and rook have been moved
        if(qsb)
        {
            //check if positions between king and rook are empty
            if(board[0][1] == ' ' && board[0][2] == ' ' && board[0][3] == ' ')
            {
                //check if king is in check and the two other spots are threatened
                if((ifCheck(board, 1)) == 0 && (isThreatened(board, "c8", 1)) == 0 && (isThreatened(board, "d8", 1)) == 0)
                {
                    return 1;
                }
            } 
        }
    }
    //end of castling  
    return 0; 
}

//captures and promotions for white pawn
int CheckCapture_wpawn(char board[8][8], int index_x, int index_y, int new_x, int new_y, char op_cp[3], char op_np[3])
{
    /* 1 for legal move, 0 for illegal move*/
	/*White Pawn*/
    if(index_y != new_y)
    {
        if(board[new_x][new_y] != ' ')
        {
            if(board[new_x][new_y] == 'p' || board[new_x][new_y] == 'r' || board[new_x][new_y] == 'n' || board[new_x][new_y] == 'b' || board[new_x][new_y] == 'q')
            {
                return 1;
            }
        }
        else
        {
            //en passant
            if(op_cp[1] == '7' && op_np[1] == '5' && position_to_piece(board, op_np) == 'p')
            {
                if(position_to_y(op_np) == new_y && index_x == 3 && new_x == 2)
                {
                    return 1;
                }                                           
            }
        }
    }
    //promotion
    else if(index_x == 1 && new_x == 0 && board[new_x][new_y] == ' ')
    {
        return 1;
    }                                                        
	
	return 0;
}

//captures and promotions for black pawn
int CheckCapture_bpawn(char board[8][8], int index_x, int index_y, int new_x, int new_y, char op_cp[3], char op_np[3])
{
    /* 1 for legal move, 0 for illegal move*/
    /* Black Pawn */
    /*Capturing*/
    if(index_y != new_y)
    {
        if(board[new_x][new_y] != ' ')
        {
            if(board[new_x][new_y] == 'P' || board[new_x][new_y] == 'R' || board[new_x][new_y] == 'N' || board[new_x][new_y] == 'B' || board[new_x][new_y] == 'Q')
            {
                return 1;
            }
        }
        else
        {
            //en passant
            if(op_cp[1] == '2' && op_np[1] == '4' && position_to_piece(board, op_np) == 'P')
            {
                if(position_to_y(op_np) == new_y && index_x == 4 && new_x == 5)
                {
                    return 1;
                }                                           
            }
        }	
    }
    //promotion
    else if(index_x == 6 && new_x == 7 && board[new_x][new_y] == ' ')
    {
        return 1;
    } 
					
	return 0;
}

//generate all pseudo-legal moves
int moveGen(char board[8][8], char all_moves[256][6], char op_cp[3], char op_np[3], int ksw, int qsw, int ksb, int qsb, int color)
{
    char piece_positions[16][3];
    int index = 0; 
    int length;
    char piece;
    int index_x;
    int index_y;
    char string[3];
    int x, y;

    switch(color)
    {
        case 1:
        {
            length = all_positions(board, piece_positions, 1);
            for(int i = 0; i < length; i++)
            {
                index_x = position_to_x(piece_positions[i]);
                index_y = position_to_y(piece_positions[i]);
                piece = board[index_x][index_y];
                switch(piece)
                {
                    case 'p':
                    {
                        for(y = index_y - 1; y < index_y + 2; y++)
                        {
                            if(y & 8) //stop when out of board
                            {
                                continue;
                            }
                            if(CheckMove_bpawn(board, index_x, index_y, index_x + 1, y, op_cp, op_np))  
                            { 
                                //promotions
                                if((index_x + 1) == 7)
                                {
                                    strncpy(string, piece_positions[i], 3);
                                    strncat(string, positions[index_x + 1][y], 3);
                                    strncpy(all_moves[index], string, 6);
                                    all_moves[index][4] = 'q';
                                    all_moves[index][5] = '\0';
                                    index++;
                                    strncpy(all_moves[index], string, 6);
                                    all_moves[index][4] = 'r';
                                    all_moves[index][5] = '\0';
                                    index++;
                                    strncpy(all_moves[index], string, 6);
                                    all_moves[index][4] = 'b';
                                    all_moves[index][5] = '\0';
                                    index++;
                                    strncpy(all_moves[index], string, 6);
                                    all_moves[index][4] = 'n';
                                    all_moves[index][5] = '\0';
                                    index++;
                                }
                                else
                                {
                                    strncpy(string, piece_positions[i], 3);
                                    strncat(string, positions[index_x + 1][y], 3);
                                    strncpy(all_moves[index], string, 6);
                                    index++;
                                }
                            }
                        }
                        //2 steps at starting position
                        if(index_x == 1)
                        {
                            if(board[2][index_y] == ' ' && board[3][index_y] == ' ')  
                            { 
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[3][index_y], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                        }
                        break;
                    }
                    case 'n':
                    {
                        for(int j = 0; j < 8; j++)
                        {
                            x = index_x + knight_moves_x[j];
                            y = index_y + knight_moves_y[j];
                            if(x & 8 || y & 8) //stop when out of board
                            {
                                continue;
                            }
                            if(board[x][y] == ' ' || board[x][y] == 'P' || board[x][y] == 'R' || board[x][y] == 'N' || board[x][y] == 'B' || board[x][y] == 'Q')  
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[x][y], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                        }
                        break;
                    }
                    case 'k':
                    {
                        for(int j = 0; j < 10; j++)
                        {
                            x = index_x + king_moves_x[j];
                            y = index_y + king_moves_y[j];
                            if(x & 8 || y & 8) //stop when out of board
                            {
                                continue;
                            }
                            if(CheckMove_bking(board, index_x, index_y, x, y, ksb, qsb))  
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[x][y], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                        }
                        break;
                    }
                    case 'r':
                    {
                        //up
                        for(int j = index_x-1; j >= 0; j--)
                        {  
                            if(board[j][index_y] == ' ' || board[j][index_y] == 'P' || board[j][index_y] == 'R' || board[j][index_y] == 'N' || board[j][index_y] == 'B' || board[j][index_y] == 'Q')
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[j][index_y], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[j][index_y] != ' ')
                            {
                                break;
                            }
                        }
                        //left
                        for(int j = index_y-1; j >= 0; j--)
                        {
                            if(board[index_x][j] == ' ' || board[index_x][j] == 'P' || board[index_x][j] == 'R' || board[index_x][j] == 'N' || board[index_x][j] == 'B' || board[index_x][j] == 'Q')
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x][j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x][j] != ' ')
                            {
                                break;
                            }
                        }
                        //right
                        for(int j = index_y+1; j <= 7; j++)
                        {
                            if(board[index_x][j] == ' ' || board[index_x][j] == 'P' || board[index_x][j] == 'R' || board[index_x][j] == 'N' || board[index_x][j] == 'B' || board[index_x][j] == 'Q')  
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x][j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x][j] != ' ')
                            {
                                break;
                            }
                        }
                        //down
                        for(int j = index_x+1; j <= 7; j++)
                        {
                            if(board[j][index_y] == ' ' || board[j][index_y] == 'P' || board[j][index_y] == 'R' || board[j][index_y] == 'N' || board[j][index_y] == 'B' || board[j][index_y] == 'Q') 
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[j][index_y], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[j][index_y] != ' ')
                            {
                                break;
                            }
                        }
                        break;
                    }
                    case 'b':
                    {
                        //up left
                        for(int j = 1; j <= 7; j++)
                        {
                            if(index_x - j < 0 || index_y - j < 0)
                            {
                                break;
                            }
                            if(board[index_x - j][index_y - j] == ' ' || board[index_x - j][index_y - j] == 'P' || board[index_x - j][index_y - j] == 'R' || board[index_x - j][index_y - j] == 'N' || board[index_x - j][index_y - j] == 'B' || board[index_x - j][index_y - j] == 'Q')  
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x - j][index_y - j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x - j][index_y - j] != ' ')
                            {
                                break;
                            }
                        }
                        //up right
                        for(int j = 1; j <= 7; j++)
                        {
                            if(index_x - j < 0 || index_y + j > 7)
                            {
                                break;
                            }
                            if(board[index_x - j][index_y + j] == ' ' || board[index_x - j][index_y + j] == 'P' || board[index_x - j][index_y + j] == 'R' || board[index_x - j][index_y + j] == 'N' || board[index_x - j][index_y + j] == 'B' || board[index_x - j][index_y + j] == 'Q') 
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x - j][index_y + j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x - j][index_y + j] != ' ')
                            {
                                break;
                            }
                        }
                        //down left
                        for(int j = 1; j <= 7; j++)
                        {
                            if(index_x + j > 7 || index_y - j < 0)
                            {
                                break;
                            }
                            if(board[index_x + j][index_y - j] == ' ' || board[index_x + j][index_y - j] == 'P' || board[index_x + j][index_y - j] == 'R' || board[index_x + j][index_y - j] == 'N' || board[index_x + j][index_y - j] == 'B' || board[index_x + j][index_y - j] == 'Q')    
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x + j][index_y - j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x + j][index_y - j] != ' ')
                            {
                                break;
                            }
                        }
                        //down right
                        for(int j = 1; j <= 7; j++)
                        {
                            if(index_x + j > 7 || index_y + j > 7)
                            {
                                break;
                            }
                            if(board[index_x + j][index_y + j] == ' ' || board[index_x + j][index_y + j] == 'P' || board[index_x + j][index_y + j] == 'R' || board[index_x + j][index_y + j] == 'N' || board[index_x + j][index_y + j] == 'B' || board[index_x + j][index_y + j] == 'Q')   
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x + j][index_y + j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x + j][index_y + j] != ' ')
                            {
                                break;
                            }
                        }
                        break;
                    }
                    case 'q':
                    {
                        //up left
                        for(int j = 1; j <= 7; j++)
                        {
                            if(index_x - j < 0 || index_y - j < 0)
                            {
                                break;
                            }
                            if(board[index_x - j][index_y - j] == ' ' || board[index_x - j][index_y - j] == 'P' || board[index_x - j][index_y - j] == 'R' || board[index_x - j][index_y - j] == 'N' || board[index_x - j][index_y - j] == 'B' || board[index_x - j][index_y - j] == 'Q')  
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x - j][index_y - j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x - j][index_y - j] != ' ')
                            {
                                break;
                            }
                        }
                        //up
                        for(int j = index_x-1; j >= 0; j--)
                        {
                            if(board[j][index_y] == ' ' || board[j][index_y] == 'P' || board[j][index_y] == 'R' || board[j][index_y] == 'N' || board[j][index_y] == 'B' || board[j][index_y] == 'Q') 
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[j][index_y], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[j][index_y] != ' ')
                            {
                                break;
                            }
                        }
                        //up right
                        for(int j = 1; j <= 7; j++)
                        {
                            if(index_x - j < 0 || index_y + j > 7)
                            {
                                break;
                            }
                            if(board[index_x - j][index_y + j] == ' ' || board[index_x - j][index_y + j] == 'P' || board[index_x - j][index_y + j] == 'R' || board[index_x - j][index_y + j] == 'N' || board[index_x - j][index_y + j] == 'B' || board[index_x - j][index_y + j] == 'Q') 
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x - j][index_y + j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x - j][index_y + j] != ' ')
                            {
                                break;
                            }
                        }
                        //left
                        for(int j = index_y-1; j >= 0; j--)
                        {
                            if(board[index_x][j] == ' ' || board[index_x][j] == 'P' || board[index_x][j] == 'R' || board[index_x][j] == 'N' || board[index_x][j] == 'B' || board[index_x][j] == 'Q')
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x][j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x][j] != ' ')
                            {
                                break;
                            }
                        }
                        //right
                        for(int j = index_y+1; j <= 7; j++)
                        {
                            if(board[index_x][j] == ' ' || board[index_x][j] == 'P' || board[index_x][j] == 'R' || board[index_x][j] == 'N' || board[index_x][j] == 'B' || board[index_x][j] == 'Q') 
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x][j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x][j] != ' ')
                            {
                                break;
                            }
                        }
                        //down left
                        for(int j = 1; j <= 7; j++)
                        {
                            if(index_x + j > 7 || index_y - j < 0)
                            {
                                break;
                            }
                            if(board[index_x + j][index_y - j] == ' ' || board[index_x + j][index_y - j] == 'P' || board[index_x + j][index_y - j] == 'R' || board[index_x + j][index_y - j] == 'N' || board[index_x + j][index_y - j] == 'B' || board[index_x + j][index_y - j] == 'Q')  
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x + j][index_y - j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x + j][index_y - j] != ' ')
                            {
                                break;
                            }
                        }
                        //down
                        for(int j = index_x+1; j <= 7; j++)
                        {
                            if(board[j][index_y] == ' ' || board[j][index_y] == 'P' || board[j][index_y] == 'R' || board[j][index_y] == 'N' || board[j][index_y] == 'B' || board[j][index_y] == 'Q') 
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[j][index_y], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[j][index_y] != ' ')
                            {
                                break;
                            }
                        }
                        //down right
                        for(int j = 1; j <= 7; j++)
                        {
                            if(index_x + j > 7 || index_y + j > 7)
                            {
                                break;
                            }
                            if(board[index_x + j][index_y + j] == ' ' || board[index_x + j][index_y + j] == 'P' || board[index_x + j][index_y + j] == 'R' || board[index_x + j][index_y + j] == 'N' || board[index_x + j][index_y + j] == 'B' || board[index_x + j][index_y + j] == 'Q')   
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x + j][index_y + j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x + j][index_y + j] != ' ')
                            {
                                break;
                            }
                        }
                        break;
                    }
                }
            }
            break;
        }
        case -1:
        {
            length = all_positions(board, piece_positions, -1);
            for(int i = 0; i < length; i++)
            {
                index_x = position_to_x(piece_positions[i]);
                index_y = position_to_y(piece_positions[i]);
                piece = board[index_x][index_y];
                switch(piece)
                {
                    case 'P':
                    {
                        //2 steps at starting position
                        if(index_x == 6)
                        {
                            if(board[5][index_y] == ' ' && board[4][index_y] == ' ')  
                            { 
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[4][index_y], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                        }
                        for(y = index_y - 1; y < index_y + 2; y++)
                        {
                            if(y & 8) //stop when out of board
                            {
                                continue;
                            }
                            if(CheckMove_wpawn(board, index_x, index_y, index_x - 1, y, op_cp, op_np)) 
                            { 
                                //promotions
                                if((index_x - 1) == 0)
                                {
                                    strncpy(string, piece_positions[i], 3);
                                    strncat(string, positions[index_x - 1][y], 3);
                                    strncpy(all_moves[index], string, 6);
                                    all_moves[index][4] = 'q';
                                    all_moves[index][5] = '\0';
                                    index++;
                                    strncpy(all_moves[index], string, 6);
                                    all_moves[index][4] = 'r';
                                    all_moves[index][5] = '\0';
                                    index++;
                                    strncpy(all_moves[index], string, 6);
                                    all_moves[index][4] = 'b';
                                    all_moves[index][5] = '\0';
                                    index++;
                                    strncpy(all_moves[index], string, 6);
                                    all_moves[index][4] = 'n';
                                    all_moves[index][5] = '\0';
                                    index++;
                                }
                                else
                                {
                                    strncpy(string, piece_positions[i], 3);
                                    strncat(string, positions[index_x - 1][y], 3);
                                    strncpy(all_moves[index], string, 6);
                                    index++;
                                }
                            }
                        }
                        break;
                    }
                    case 'N':
                    {
                        for(int j = 0; j < 8; j++)
                        {
                            x = index_x + knight_moves_x[j];
                            y = index_y + knight_moves_y[j];
                            if(x & 8 || y & 8) //stop when out of board
                            {
                                continue;
                            }
                            if(board[x][y] == ' ' || board[x][y] == 'p' || board[x][y] == 'r' || board[x][y] == 'n' || board[x][y] == 'b' || board[x][y] == 'q')   
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[x][y], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                        }
                        break;
                    }
                    case 'K':
                    {
                        for(int j = 0; j < 10; j++)
                        {
                            x = index_x + king_moves_x[j];
                            y = index_y + king_moves_y[j];
                            if(x & 8 || y & 8) //stop when out of board
                            {
                                continue;
                            }
                            if(CheckMove_wking(board, index_x, index_y, x, y, ksw, qsw))  
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[x][y], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                        }
                        break;
                    }
                    case 'R':
                    {
                        //up
                        for(int j = index_x-1; j >= 0; j--)
                        {
                            if(board[j][index_y] == ' ' || board[j][index_y] == 'p' || board[j][index_y] == 'r' || board[j][index_y] == 'n' || board[j][index_y] == 'b' || board[j][index_y] == 'q') 
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[j][index_y], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[j][index_y] != ' ')
                            {
                                break;
                            }
                        }
                        //left
                        for(int j = index_y-1; j >= 0; j--)
                        {
                            if(board[index_x][j] == ' ' || board[index_x][j] == 'p' || board[index_x][j] == 'r' || board[index_x][j] == 'n' || board[index_x][j] == 'b' || board[index_x][j] == 'q') 
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x][j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x][j] != ' ')
                            {
                                break;
                            }
                        }
                        //right
                        for(int j = index_y+1; j <= 7; j++)
                        {
                            if(board[index_x][j] == ' ' || board[index_x][j] == 'p' || board[index_x][j] == 'r' || board[index_x][j] == 'n' || board[index_x][j] == 'b' || board[index_x][j] == 'q')  
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x][j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x][j] != ' ')
                            {
                                break;
                            }
                        }
                        //down
                        for(int j = index_x+1; j <= 7; j++)
                        {
                            if(board[j][index_y] == ' ' || board[j][index_y] == 'p' || board[j][index_y] == 'r' || board[j][index_y] == 'n' || board[j][index_y] == 'b' || board[j][index_y] == 'q') 
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[j][index_y], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[j][index_y] != ' ')
                            {
                                break;
                            }
                        }
                        break;
                    }
                    case 'B':
                    {
                        //up left
                        for(int j = 1; j <= 7; j++)
                        {
                            if(index_x - j < 0 || index_y - j < 0)
                            {
                                break;
                            }
                            if(board[index_x - j][index_y - j] == ' ' || board[index_x - j][index_y - j] == 'p' || board[index_x - j][index_y - j] == 'r' || board[index_x - j][index_y - j] == 'n' || board[index_x - j][index_y - j] == 'b' || board[index_x - j][index_y - j] == 'q')  
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x - j][index_y - j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x - j][index_y - j] != ' ')
                            {
                                break;
                            }
                        }
                        //up right
                        for(int j = 1; j <= 7; j++)
                        {
                            if(index_x - j < 0 || index_y + j > 7)
                            {
                                break;
                            }
                            if(board[index_x - j][index_y + j] == ' ' || board[index_x - j][index_y + j] == 'p' || board[index_x - j][index_y + j] == 'r' || board[index_x - j][index_y + j] == 'n' || board[index_x - j][index_y + j] == 'b' || board[index_x - j][index_y + j] == 'q')   
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x - j][index_y + j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x - j][index_y + j] != ' ')
                            {
                                break;
                            }
                        }
                        //down left
                        for(int j = 1; j <= 7; j++)
                        {
                            if(index_x + j > 7 || index_y - j < 0)
                            {
                                break;
                            }
                            if(board[index_x + j][index_y - j] == ' ' || board[index_x + j][index_y - j] == 'p' || board[index_x + j][index_y - j] == 'r' || board[index_x + j][index_y - j] == 'n' || board[index_x + j][index_y - j] == 'b' || board[index_x + j][index_y - j] == 'q')   
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x + j][index_y - j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x + j][index_y - j] != ' ')
                            {
                                break;
                            }
                        }
                        //down right
                        for(int j = 1; j <= 7; j++)
                        {
                            if(index_x + j > 7 || index_y + j > 7)
                            {
                                break;
                            }
                            if(board[index_x + j][index_y + j] == ' ' || board[index_x + j][index_y + j] == 'p' || board[index_x + j][index_y + j] == 'r' || board[index_x + j][index_y + j] == 'n' || board[index_x + j][index_y + j] == 'b' || board[index_x + j][index_y + j] == 'q')   
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x + j][index_y + j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x + j][index_y + j] != ' ')
                            {
                                break;
                            }
                        }
                        break;
                    }
                    case 'Q':
                    {
                        //up left
                        for(int j = 1; j <= 7; j++)
                        {
                            if(index_x - j < 0 || index_y - j < 0)
                            {
                                break;
                            }
                            if(board[index_x - j][index_y - j] == ' ' || board[index_x - j][index_y - j] == 'p' || board[index_x - j][index_y - j] == 'r' || board[index_x - j][index_y - j] == 'n' || board[index_x - j][index_y - j] == 'b' || board[index_x - j][index_y - j] == 'q')  
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x - j][index_y - j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x - j][index_y - j] != ' ')
                            {
                                break;
                            }
                        }
                        //up
                        for(int j = index_x-1; j >= 0; j--)
                        {
                            if(board[j][index_y] == ' ' || board[j][index_y] == 'p' || board[j][index_y] == 'r' || board[j][index_y] == 'n' || board[j][index_y] == 'b' || board[j][index_y] == 'q')  
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[j][index_y], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[j][index_y] != ' ')
                            {
                                break;
                            }
                        }
                        //up right
                        for(int j = 1; j <= 7; j++)
                        {
                            if(index_x - j < 0 || index_y + j > 7)
                            {
                                break;
                            }
                            if(board[index_x - j][index_y + j] == ' ' || board[index_x - j][index_y + j] == 'p' || board[index_x - j][index_y + j] == 'r' || board[index_x - j][index_y + j] == 'n' || board[index_x - j][index_y + j] == 'b' || board[index_x - j][index_y + j] == 'q')   
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x - j][index_y + j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x - j][index_y + j] != ' ')
                            {
                                break;
                            }
                        }
                        //left
                        for(int j = index_y-1; j >= 0; j--)
                        {
                            if(board[index_x][j] == ' ' || board[index_x][j] == 'p' || board[index_x][j] == 'r' || board[index_x][j] == 'n' || board[index_x][j] == 'b' || board[index_x][j] == 'q')   
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x][j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x][j] != ' ')
                            {
                                break;
                            }
                        }
                        //right
                        for(int j = index_y+1; j <= 7; j++)
                        {
                            if(board[index_x][j] == ' ' || board[index_x][j] == 'p' || board[index_x][j] == 'r' || board[index_x][j] == 'n' || board[index_x][j] == 'b' || board[index_x][j] == 'q')   
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x][j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x][j] != ' ')
                            {
                                break;
                            }
                        }
                        //down left
                        for(int j = 1; j <= 7; j++)
                        {
                            if(index_x + j > 7 || index_y - j < 0)
                            {
                                break;
                            }
                            if(board[index_x + j][index_y - j] == ' ' || board[index_x + j][index_y - j] == 'p' || board[index_x + j][index_y - j] == 'r' || board[index_x + j][index_y - j] == 'n' || board[index_x + j][index_y - j] == 'b' || board[index_x + j][index_y - j] == 'q')   
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x + j][index_y - j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x + j][index_y - j] != ' ')
                            {
                                break;
                            }
                        }
                        //down
                        for(int j = index_x+1; j <= 7; j++)
                        {
                            if(board[j][index_y] == ' ' || board[j][index_y] == 'p' || board[j][index_y] == 'r' || board[j][index_y] == 'n' || board[j][index_y] == 'b' || board[j][index_y] == 'q') 
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[j][index_y], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[j][index_y] != ' ')
                            {
                                break;
                            }
                        }
                        //down right
                        for(int j = 1; j <= 7; j++)
                        {
                            if(index_x + j > 7 || index_y + j > 7)
                            {
                                break;
                            }
                            if(board[index_x + j][index_y + j] == ' ' || board[index_x + j][index_y + j] == 'p' || board[index_x + j][index_y + j] == 'r' || board[index_x + j][index_y + j] == 'n' || board[index_x + j][index_y + j] == 'b' || board[index_x + j][index_y + j] == 'q')   
                            {  
                                strncpy(string, piece_positions[i], 3);
                                strncat(string, positions[index_x + j][index_y + j], 3);
                                strncpy(all_moves[index], string, 6);
                                index++;
                            }
                            if(board[index_x + j][index_y + j] != ' ')
                            {
                                break;
                            }
                        }
                        break;
                    }
                }
            }
            break;
        }
        
    }
    
    return index;
}

static unsigned long long int perft(char board[8][8], int depth, int color, char op_cp[3], char op_np[3], int ksw, int qsw, int ksb, int qsb)
{   
    unsigned long long int nodes = 0;
    int length;
    char board_copy[8][8];
    char cp[3];
    char np[3];
    int ksb2;
    int qsb2;
    int ksw2;
    int qsw2;
    char promotion;
    char moved_piece;
    char piece;

    if(depth == 0)
        return 1ULL;
    
    //get children of node
    char moves[256][6];
    length = moveGen(board, moves, op_cp, op_np, ksw, qsw, ksb, qsb, color);
    //assert(length <= 256);
    
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
        makeMove(cp, np, promotion, moved_piece, piece, board_copy);
        //int isprom_ep = makeMove(cp, np, promotion, piece, board);
        
        //check if check is ignored
        if(ifCheck(board_copy, color))
        {
            //undoMove(cp, np, isprom_ep, piece, color, board);
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
        
        nodes += perft(board_copy, depth - 1, -color, cp, np, ksw2, qsw2, ksb2, qsb2);
        //undoMove(cp, np, isprom_ep, piece, color, board);
    }
    return nodes;
}

int main()
{
    int nodes = perft(board2, 5, -1, "", "", 1, 1, 1, 1);
    printf("nodes count: %lld\n", nodes);
    return 0;
}