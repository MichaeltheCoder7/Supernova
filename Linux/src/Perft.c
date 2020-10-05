#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>

unsigned long long int table[8][8][12];
unsigned long long int turn;
unsigned long long int kswcr; 
unsigned long long int qswcr;
unsigned long long int ksbcr;
unsigned long long int qsbcr;
unsigned long long int ep[8];

//position 1
//depth 5: 4865609 nodes
//depth 6: 119060324 nodes
char board1[8][8] = {
						
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

int king_moves_x[8] = { -1, -1, -1,  0,  0,  1,  1,  1 };
int king_moves_y[8] = { -1,  0,  1, -1,  1, -1,  0,  1 };

enum{ wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK };

enum positions {
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1,
};

int piece_list[12][10] = {

                    { a2, b2, c2, d2, e2, f2, g2, h2, -1, -1 }, //wP
                    { b1, g1, -1, -1, -1, -1, -1, -1, -1, -1 }, //wN
                    { c1, f1, -1, -1, -1, -1, -1, -1, -1, -1 }, //wB
                    { a1, h1, -1, -1, -1, -1, -1, -1, -1, -1 }, //wR
                    { d1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, //wQ
                    { e1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, //wK
                    { a7, b7, c7, d7, e7, f7, g7, h7, -1, -1 }, //bP
                    { b8, g8, -1, -1, -1, -1, -1, -1, -1, -1 }, //bN
                    { c8, f8, -1, -1, -1, -1, -1, -1, -1, -1 }, //bB
                    { a8, h8, -1, -1, -1, -1, -1, -1, -1, -1 }, //bR
                    { d8, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, //bQ
                    { e8, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, //bK

                    };

int piece_count[12] = { 8, 2, 2, 2, 1, 1, 8, 2, 2, 2, 1, 1 };

int index_board[64] = {
	  0,   0,   0,   0,   0,   1,   1,   1,
	  0,   1,   2,   3,   4,   5,   6,   7,
	 -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
	 -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
	 -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
	 -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
	  0,   1,   2,   3,   4,   5,   6,   7,
	  0,   0,   0,   0,   0,   1,   1,   1
};

int piece_list2[12][10] = {

                    { a2, b2, c2, d5, e4, f2, g2, h2, -1, -1 }, //wP
                    { c3, e5, -1, -1, -1, -1, -1, -1, -1, -1 }, //wN
                    { d2, e2, -1, -1, -1, -1, -1, -1, -1, -1 }, //wB
                    { a1, h1, -1, -1, -1, -1, -1, -1, -1, -1 }, //wR
                    { f3, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, //wQ
                    { e1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, //wK
                    { a7, b4, c7, d7, e6, f7, g6, h3, -1, -1 }, //bP
                    { b6, f6, -1, -1, -1, -1, -1, -1, -1, -1 }, //bN
                    { a6, g7, -1, -1, -1, -1, -1, -1, -1, -1 }, //bB
                    { a8, h8, -1, -1, -1, -1, -1, -1, -1, -1 }, //bR
                    { e7, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, //bQ
                    { e8, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, //bK

                    };

int piece_count2[12] = { 8, 2, 2, 2, 1, 1, 8, 2, 2, 2, 1, 1 };

int index_board2[64] = {
	  0,  -1,  -1,  -1,   0,  -1,  -1,   1,
	  0,  -1,   2,   3,   0,   5,   1,  -1,
	  0,   0,  -1,  -1,   4,   1,   6,  -1,
	 -1,  -1,  -1,   3,   1,  -1,  -1,  -1,
	 -1,   1,  -1,  -1,   4,  -1,  -1,  -1,
	 -1,  -1,   0,  -1,  -1,   0,  -1,   7,
	  0,   1,   2,   0,   1,   5,   6,   7,
	  0,  -1,  -1,  -1,   0,  -1,  -1,   1
};

typedef struct{

    unsigned long long int key;
    char board[8][8];
    int ksw;
    int qsw;
    int ksb;
    int qsb;
    int ep_file;
    int piece_num;
    int halfmove_counter;
    int piece_list[12][10];
    int piece_count[12];
    int index_board[64];

} BOARD;

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

int piece_code(char piece) 
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

//return 1 for promotion
//return 2 for capture
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
        isCapture = 2;
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
                //white Pawn Promotion
                switch(promotion)
                {
                    case 'r':
                        pos->board[0][new_y] = 'R';
                        pos->key ^= table[0][new_y][0];
                        pos->key ^= table[0][new_y][3];
                        //add piece
                        pos->piece_list[wR][pos->piece_count[wR]] = new_64;
                        pos->index_board[new_64] = pos->piece_count[wR];
                        pos->piece_count[wR]++;
                        break;
                    case 'b':
                        pos->board[0][new_y] = 'B';
                        pos->key ^= table[0][new_y][0];
                        pos->key ^= table[0][new_y][2];
                        //add piece
                        pos->piece_list[wB][pos->piece_count[wB]] = new_64;
                        pos->index_board[new_64] = pos->piece_count[wB];
                        pos->piece_count[wB]++;
                        break;
                    case 'n':
                        pos->board[0][new_y] = 'N';
                        pos->key ^= table[0][new_y][0];
                        pos->key ^= table[0][new_y][1];
                        //add piece
                        pos->piece_list[wN][pos->piece_count[wN]] = new_64;
                        pos->index_board[new_64] = pos->piece_count[wN];
                        pos->piece_count[wN]++;
                        break;
                    default:
                        pos->board[0][new_y] = 'Q';
                        pos->key ^= table[0][new_y][0];
                        pos->key ^= table[0][new_y][4];
                        //add piece
                        pos->piece_list[wQ][pos->piece_count[wQ]] = new_64;
                        pos->index_board[new_64] = pos->piece_count[wQ];
                        pos->piece_count[wQ]++;
                        break;
                }
                return 1;
            }
            //en passant
            else if(abs(new_p[0] - cur_p[0]) == 1 && op_piece == ' ')
            {
                pos->board[cur_x][new_y] = ' ';
                pos->key ^= table[cur_x][new_y][6];
                pos->piece_num--;
                //piece list update
                //remove pawn
                pos->piece_count[bP]--;
                index = pos->index_board[8*cur_x + new_y];
                square = pos->piece_list[bP][pos->piece_count[bP]];
                pos->piece_list[bP][index] = square;
                pos->index_board[square] = index;
                return 2;
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
                //black Pawn Promotion 
                switch(promotion)
                {
                    case 'r':
                        pos->board[7][new_y] = 'r';
                        pos->key ^= table[7][new_y][6];
                        pos->key ^= table[7][new_y][9];
                        //add piece
                        pos->piece_list[bR][pos->piece_count[bR]] = new_64;
                        pos->index_board[new_64] = pos->piece_count[bR];
                        pos->piece_count[bR]++;
                        break;
                    case 'b':
                        pos->board[7][new_y] = 'b';
                        pos->key ^= table[7][new_y][6];
                        pos->key ^= table[7][new_y][8];
                        //add piece
                        pos->piece_list[bB][pos->piece_count[bB]] = new_64;
                        pos->index_board[new_64] = pos->piece_count[bB];
                        pos->piece_count[bB]++;
                        break;
                    case 'n':
                        pos->board[7][new_y] = 'n';
                        pos->key ^= table[7][new_y][6];
                        pos->key ^= table[7][new_y][7];
                        //add piece
                        pos->piece_list[bN][pos->piece_count[bN]] = new_64;
                        pos->index_board[new_64] = pos->piece_count[bN];
                        pos->piece_count[bN]++;
                        break;
                    default:
                        pos->board[7][new_y] = 'q';
                        pos->key ^= table[7][new_y][6];
                        pos->key ^= table[7][new_y][10];
                        //add piece
                        pos->piece_list[bQ][pos->piece_count[bQ]] = new_64;
                        pos->index_board[new_64] = pos->piece_count[bQ];
                        pos->piece_count[bQ]++;
                        break;
                }
                return 1;
            }
            //en passant
            else if(abs(new_p[0] - cur_p[0]) == 1 && op_piece == ' ')
            {
                pos->board[cur_x][new_y] = ' ';
                pos->key ^= table[cur_x][new_y][0];
                pos->piece_num--;
                //piece list update
                //remove pawn
                pos->piece_count[wP]--;
                index = pos->index_board[8*cur_x + new_y];
                square = pos->piece_list[wP][pos->piece_count[wP]];
                pos->piece_list[wP][index] = square;
                pos->index_board[square] = index;
                return 2;
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
            }
            break;
    }
    
    return isCapture;
}

int ifCheck(BOARD *pos, int color)
{
    int king_x = -1;
    int king_y = -1;

    switch(color)
    {
        case 1:
        {
            king_x = pos->piece_list[bK][0] / 8;
            king_y = pos->piece_list[bK][0] % 8;
            assert(king_x >= 0);
            //down left
            for(int i = 1; i <= 7; i++)
            {
                if(king_x + i > 7 || king_y - i < 0)
                {
                    break;
                }
                if(pos->board[king_x + i][king_y - i] != ' ')
                {
                    if(i == 1 && (pos->board[king_x + i][king_y - i] == 'P' || pos->board[king_x + i][king_y - i] == 'K'))
                    {
                        return 1;
                    }
                    if(pos->board[king_x + i][king_y - i] == 'B' || pos->board[king_x + i][king_y - i] == 'Q')
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
                if(pos->board[king_x + i][king_y + i] != ' ')
                {
                    if(i == 1 && (pos->board[king_x + i][king_y + i] == 'P' || pos->board[king_x + i][king_y + i] == 'K'))
                    {
                        return 1;
                    }
                    if(pos->board[king_x + i][king_y + i] == 'B' || pos->board[king_x + i][king_y + i] == 'Q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //knight attacks
            if(king_x + 2 <= 7 && king_y + 1 <= 7)
            {
                if(pos->board[king_x + 2][king_y + 1] == 'N')
                    return 1;
            }
            if(king_x + 2 <= 7 && king_y - 1 >= 0)
            {
                if(pos->board[king_x + 2][king_y - 1] == 'N')
                    return 1;
            }
            if(king_x + 1 <= 7 && king_y + 2 <= 7)
            {
                if(pos->board[king_x + 1][king_y + 2] == 'N')
                    return 1;
            }
            if(king_x + 1 <= 7 && king_y - 2 >= 0)
            {
                if(pos->board[king_x + 1][king_y - 2] == 'N')
                    return 1;
            }
            if(king_x - 2 >= 0 && king_y + 1 <= 7)
            {
                if(pos->board[king_x - 2][king_y + 1] == 'N')
                    return 1;
            }
            if(king_x - 2 >= 0 && king_y - 1 >= 0)
            {
                if(pos->board[king_x - 2][king_y - 1] == 'N')
                    return 1;
            }
            if(king_x - 1 >= 0 && king_y + 2 <= 7)
            {
                if(pos->board[king_x - 1][king_y + 2] == 'N')
                    return 1;
            }
            if(king_x - 1 >= 0 && king_y - 2 >= 0)
            {
                if(pos->board[king_x - 1][king_y - 2] == 'N')
                    return 1;
            }
            //down
            for(int i = king_x+1; i <= 7; i++)
            {
                if(pos->board[i][king_y] != ' ')
                {
                    if(i == king_x+1 && pos->board[i][king_y] == 'K')
                    {
                        return 1;
                    }
                    if(pos->board[i][king_y] == 'R' || pos->board[i][king_y] == 'Q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //left
            for(int i = king_y-1; i >= 0; i--)
            {
                if(pos->board[king_x][i] != ' ')
                {
                    if(i == king_y-1 && pos->board[king_x][i] == 'K')
                    {
                        return 1;
                    }
                    if(pos->board[king_x][i] == 'R' || pos->board[king_x][i] == 'Q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //right
            for(int i = king_y+1; i <= 7; i++)
            {
                if(pos->board[king_x][i] != ' ')
                {
                    if(i == king_y+1 && pos->board[king_x][i] == 'K')
                    {
                        return 1;
                    }
                    if(pos->board[king_x][i] == 'R' || pos->board[king_x][i] == 'Q')
                    {
                        return 1;
                    }
                    break;
                }
            }     
            //up
            for(int i = king_x-1; i >= 0; i--)
            {
                if(pos->board[i][king_y] != ' ')
                {
                    if(i == king_x-1 && pos->board[i][king_y] == 'K')
                    {
                        return 1;
                    }
                    if(pos->board[i][king_y] == 'R' || pos->board[i][king_y] == 'Q')
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
                if(pos->board[king_x - i][king_y - i] != ' ')
                {
                    if(i == 1 && pos->board[king_x - i][king_y - i] == 'K')
                    {
                        return 1;
                    }
                    if(pos->board[king_x - i][king_y - i] == 'B' || pos->board[king_x - i][king_y - i] == 'Q')
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
                if(pos->board[king_x - i][king_y + i] != ' ')
                {
                    if(i == 1 && pos->board[king_x - i][king_y + i] == 'K')
                    {
                        return 1;
                    }
                    if(pos->board[king_x - i][king_y + i] == 'B' || pos->board[king_x - i][king_y + i] == 'Q')
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
            king_x = pos->piece_list[wK][0] / 8;
            king_y = pos->piece_list[wK][0] % 8;
            assert(king_x >= 0);
            //up left
            for(int i = 1; i <= 7; i++)
            {
                if(king_x - i < 0 || king_y - i < 0)
                {
                    break;
                }
                if(pos->board[king_x - i][king_y - i] != ' ')
                {
                    if(i == 1 && (pos->board[king_x - i][king_y - i] == 'p' || pos->board[king_x - i][king_y - i] == 'k'))
                    {
                        return 1;
                    }
                    if(pos->board[king_x - i][king_y - i] == 'b' || pos->board[king_x - i][king_y - i] == 'q')
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
                if(pos->board[king_x - i][king_y + i] != ' ')
                {
                    if(i == 1 && (pos->board[king_x - i][king_y + i] == 'p' || pos->board[king_x - i][king_y + i] == 'k'))
                    {
                        return 1;
                    }
                    if(pos->board[king_x - i][king_y + i] == 'b' || pos->board[king_x - i][king_y + i] == 'q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //knight attacks
            if(king_x - 2 >= 0 && king_y + 1 <= 7)
            {
                if(pos->board[king_x - 2][king_y + 1] == 'n')
                    return 1;
            }
            if(king_x - 2 >= 0 && king_y - 1 >= 0)
            {
                if(pos->board[king_x - 2][king_y - 1] == 'n')
                    return 1;
            }
            if(king_x - 1 >= 0 && king_y + 2 <= 7)
            {
                if(pos->board[king_x - 1][king_y + 2] == 'n')
                    return 1;
            }
            if(king_x - 1 >= 0 && king_y - 2 >= 0)
            {
                if(pos->board[king_x - 1][king_y - 2] == 'n')
                    return 1;
            }
            if(king_x + 2 <= 7 && king_y + 1 <= 7)
            {
                if(pos->board[king_x + 2][king_y + 1] == 'n')
                    return 1;
            }
            if(king_x + 2 <= 7 && king_y - 1 >= 0)
            {
                if(pos->board[king_x + 2][king_y - 1] == 'n')
                    return 1;
            }
            if(king_x + 1 <= 7 && king_y + 2 <= 7)
            {
                if(pos->board[king_x + 1][king_y + 2] == 'n')
                    return 1;
            }
            if(king_x + 1 <= 7 && king_y - 2 >= 0)
            {
                if(pos->board[king_x + 1][king_y - 2] == 'n')
                    return 1;
            }
            //up
            for(int i = king_x-1; i >= 0; i--)
            {
                if(pos->board[i][king_y] != ' ')
                {
                    if(i == king_x-1 && pos->board[i][king_y] == 'k')
                    {
                        return 1;
                    }
                    if(pos->board[i][king_y] == 'r' || pos->board[i][king_y] == 'q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //left
            for(int i = king_y-1; i >= 0; i--)
            {
                if(pos->board[king_x][i] != ' ')
                {
                    if(i == king_y-1 && pos->board[king_x][i] == 'k')
                    {
                        return 1;
                    }
                    if(pos->board[king_x][i] == 'r' || pos->board[king_x][i] == 'q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //right
            for(int i = king_y+1; i <= 7; i++)
            {
                if(pos->board[king_x][i] != ' ')
                {
                    if(i == king_y+1 && pos->board[king_x][i] == 'k')
                    {
                        return 1;
                    }
                    if(pos->board[king_x][i] == 'r' || pos->board[king_x][i] == 'q')
                    {
                        return 1;
                    }
                    break;
                }
            }
            //down
            for(int i = king_x+1; i <= 7; i++)
            {
                if(pos->board[i][king_y] != ' ')
                {
                    if(i == king_x+1 && pos->board[i][king_y] == 'k')
                    {
                        return 1;
                    }
                    if(pos->board[i][king_y] == 'r' || pos->board[i][king_y] == 'q')
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
                if(pos->board[king_x + i][king_y - i] != ' ')
                {
                    if(i == 1 && pos->board[king_x + i][king_y - i] == 'k')
                    {
                        return 1;
                    }
                    if(pos->board[king_x + i][king_y - i] == 'b' || pos->board[king_x + i][king_y - i] == 'q')
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
                if(pos->board[king_x + i][king_y + i] != ' ')
                {
                    if(i == 1 && pos->board[king_x + i][king_y + i] == 'k')
                    {
                        return 1;
                    }
                    if(pos->board[king_x + i][king_y + i] == 'b' || pos->board[king_x + i][king_y + i] == 'q')
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
            break;
        }
        
        case -1:
        {
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
            break;
        }
    }
    
    return 0;
}

//checking moves for white pawn
int CheckMove_wpawn(BOARD *pos, int index_y, int new_x, int new_y)
{
    /* 1 for legal move, 0 for illegal move*/
    /*White Pawn*/
    /*Capturing*/
    if(index_y != new_y)
    {
        if(pos->board[new_x][new_y] != ' ')
        {
            if(pos->board[new_x][new_y] == 'p' || pos->board[new_x][new_y] == 'r' || pos->board[new_x][new_y] == 'n' || pos->board[new_x][new_y] == 'b' || pos->board[new_x][new_y] == 'q')
            {
                return 1;
            }
        }
        else
        {
            //en passant
            if(pos->ep_file && pos->ep_file == new_y + 1)
            {
                if(new_x == 2)
                {
                    return 1;
                }                                           
            }
        }
    }
    /*one step forward*/
    else
    {
        if(pos->board[new_x][new_y] == ' ')
        {
            return 1;
        }
    }  
    return 0; 
}                                                     

//checking moves for black pawn
int CheckMove_bpawn(BOARD *pos, int index_y, int new_x, int new_y)
{
    /* 1 for legal move, 0 for illegal move*/
    /* Black Pawn */
    /*Capturing*/
    if(index_y != new_y)
    {
        if(pos->board[new_x][new_y] != ' ')
        {
            if(pos->board[new_x][new_y] == 'P' || pos->board[new_x][new_y] == 'R' || pos->board[new_x][new_y] == 'N' || pos->board[new_x][new_y] == 'B' || pos->board[new_x][new_y] == 'Q')
            {
                return 1;
            }
        }
        else
        {
            //en passant
            if(pos->ep_file && pos->ep_file == new_y + 1)
            {
                if(new_x == 5)
                {
                    return 1;
                }                                           
            }
                                
        }	
    }
    /*one step forward*/
    else
    {
        if(pos->board[new_x][new_y] == ' ')
        {
            return 1;
        }
    }  
    return 0; 
} 

//castling for white king
int CheckMove_wkingside(BOARD *pos)
{
    /* 1 for legal move, 0 for illegal move*/
    /*White King*/
    //castling
    //king side
    //check if king and rook have been moved
    if(pos->ksw)
    {
        //check if positions between king and rook are empty
        if(pos->board[7][5] == ' ' && pos->board[7][6] == ' ')
        {
            //check if king is in check and the two other spots are threatened
            if(!ifCheck(pos, -1) && !isThreatened(pos->board, "f1", -1) && !isThreatened(pos->board, "g1", -1))
            {
                return 1;
            }
        }   
    }   
    return 0; 
}

int CheckMove_wqueenside(BOARD *pos)
{
    /* 1 for legal move, 0 for illegal move*/
    /*White King*/
    //castling
    //queen side
    //check if king and rook have been moved
    if(pos->qsw)
    {
        //check if positions between king and rook are empty
        if(pos->board[7][1] == ' ' && pos->board[7][2] == ' ' && pos->board[7][3] == ' ')
        {
            //check if king is in check and the two other spots are threatened
            if(!ifCheck(pos, -1) && !isThreatened(pos->board, "c1", -1) && !isThreatened(pos->board, "d1", -1))
            {
                return 1;      
            }
        } 
    }
    //end of castling  
    return 0; 
}

//castling for black king
int CheckMove_bkingside(BOARD *pos)
{
    /* 1 for legal move, 0 for illegal move*/
    /*Black King*/
    //castling
    //king side
    //check if king and rook have been moved
    if(pos->ksb)
    {
        //check if positions between king and rook are empty
        if(pos->board[0][5] == ' ' && pos->board[0][6] == ' ')
        {
            //check if king is in check and the two other spots are threatened
            if(!ifCheck(pos, 1) && !isThreatened(pos->board, "f8", 1) && !isThreatened(pos->board, "g8", 1))
            {
                return 1;
            }
        }          
    }

    return 0; 
}

//castling for black king
int CheckMove_bqueenside(BOARD *pos)
{
    /* 1 for legal move, 0 for illegal move*/
    /*Black King*/
    //castling
    //queen side
    //check if king and rook have been moved
    if(pos->qsb)
    {
        //check if positions between king and rook are empty
        if(pos->board[0][1] == ' ' && pos->board[0][2] == ' ' && pos->board[0][3] == ' ')
        {
            //check if king is in check and the two other spots are threatened
            if(!ifCheck(pos, 1) && !isThreatened(pos->board, "c8", 1) && !isThreatened(pos->board, "d8", 1))
            {
                return 1;
            }
        } 
    }

    return 0; 
}

//generate all pseudo-legal moves
int moveGen(BOARD *pos, char all_moves[256][6], int color)
{
    int index = 0; 
    int index_x;
    int index_y;
    int x, y;

    switch(color)
    {
        //black pieces
        case 1:
        {
            //pawn
            for(int i = 0; i < pos->piece_count[bP]; i++)
            {
                index_x = pos->piece_list[bP][i] / 8;
                index_y = pos->piece_list[bP][i] % 8;
                for(y = index_y - 1; y < index_y + 2; y++)
                {
                    if(y & 8) //skip when out of board
                    {
                        continue;
                    }
                    if(CheckMove_bpawn(pos, index_y, index_x + 1, y))  
                    { 
                        //promotions
                        if((index_x + 1) == 7)
                        {
                            strncpy(all_moves[index], positions[index_x][index_y], 3);
                            strncat(all_moves[index], positions[index_x + 1][y], 3);
                            all_moves[index][4] = 'q';
                            all_moves[index][5] = '\0';
                            index++;
                            strncpy(all_moves[index], all_moves[index-1], 6);
                            all_moves[index][4] = 'r';
                            index++;
                            strncpy(all_moves[index], all_moves[index-1], 6);
                            all_moves[index][4] = 'b';
                            index++;
                            strncpy(all_moves[index], all_moves[index-1], 6);
                            all_moves[index][4] = 'n';
                            index++;
                        }
                        else
                        {
                            strncpy(all_moves[index], positions[index_x][index_y], 3);
                            strncat(all_moves[index], positions[index_x + 1][y], 3);
                            index++;
                        }
                    }
                }
                //2 steps at starting position
                if(index_x == 1)
                {
                    if(pos->board[2][index_y] == ' ' && pos->board[3][index_y] == ' ')  
                    { 
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[3][index_y], 3);
                        index++;
                    }
                }
            }
            //knight
            for(int i = 0; i < pos->piece_count[bN]; i++)
            {
                index_x = pos->piece_list[bN][i] / 8;
                index_y = pos->piece_list[bN][i] % 8;
                for(int j = 0; j < 8; j++)
                {
                    x = index_x + knight_moves_x[j];
                    y = index_y + knight_moves_y[j];
                    if(x & 8 || y & 8) //skips when out of board
                    {
                        continue;
                    }
                    if(pos->board[x][y] == ' ' || pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q')  
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        index++;
                    }
                }
            }
            //king
            index_x = pos->piece_list[bK][0] / 8;
            index_y = pos->piece_list[bK][0] % 8;
            //castling
            if(index_x == 0 && index_y == 4)
            {
                if(CheckMove_bkingside(pos))  
                {  
                    strncpy(all_moves[index], positions[index_x][index_y], 3);
                    strncat(all_moves[index], positions[0][6], 3);
                    index++;
                }
                if(CheckMove_bqueenside(pos))  
                {  
                    strncpy(all_moves[index], positions[index_x][index_y], 3);
                    strncat(all_moves[index], positions[0][2], 3);
                    index++;
                }
            }
            for(int j = 0; j < 8; j++)
            {
                x = index_x + king_moves_x[j];
                y = index_y + king_moves_y[j];
                if(x & 8 || y & 8) //skip when out of board
                {
                    continue;
                }
                if(pos->board[x][y] == ' ' || pos->board[x][y] == 'P' || pos->board[x][y] == 'R' || pos->board[x][y] == 'N' || pos->board[x][y] == 'B' || pos->board[x][y] == 'Q') 
                {  
                    strncpy(all_moves[index], positions[index_x][index_y], 3);
                    strncat(all_moves[index], positions[x][y], 3);
                    index++;
                }
            }
            //rook
            for(int i = 0; i < pos->piece_count[bR]; i++)
            {
                index_x = pos->piece_list[bR][i] / 8;
                index_y = pos->piece_list[bR][i] % 8;
                //up
                for(int j = index_x-1; j >= 0; j--)
                {  
                    if(pos->board[j][index_y] == ' ' || pos->board[j][index_y] == 'P' || pos->board[j][index_y] == 'R' || pos->board[j][index_y] == 'N' || pos->board[j][index_y] == 'B' || pos->board[j][index_y] == 'Q')
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
                        index++;
                    }
                    if(pos->board[j][index_y] != ' ')
                    {
                        break;
                    }
                }
                //left
                for(int j = index_y-1; j >= 0; j--)
                {
                    if(pos->board[index_x][j] == ' ' || pos->board[index_x][j] == 'P' || pos->board[index_x][j] == 'R' || pos->board[index_x][j] == 'N' || pos->board[index_x][j] == 'B' || pos->board[index_x][j] == 'Q')
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
                        index++;
                    }
                    if(pos->board[index_x][j] != ' ')
                    {
                        break;
                    }
                }
                //right
                for(int j = index_y+1; j <= 7; j++)
                {
                    if(pos->board[index_x][j] == ' ' || pos->board[index_x][j] == 'P' || pos->board[index_x][j] == 'R' || pos->board[index_x][j] == 'N' || pos->board[index_x][j] == 'B' || pos->board[index_x][j] == 'Q')  
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
                        index++;
                    }
                    if(pos->board[index_x][j] != ' ')
                    {
                        break;
                    }
                }
                //down
                for(int j = index_x+1; j <= 7; j++)
                {
                    if(pos->board[j][index_y] == ' ' || pos->board[j][index_y] == 'P' || pos->board[j][index_y] == 'R' || pos->board[j][index_y] == 'N' || pos->board[j][index_y] == 'B' || pos->board[j][index_y] == 'Q') 
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
                        index++;
                    }
                    if(pos->board[j][index_y] != ' ')
                    {
                        break;
                    }
                }
            }
            //bishop
            for(int i = 0; i < pos->piece_count[bB]; i++)
            {
                index_x = pos->piece_list[bB][i] / 8;
                index_y = pos->piece_list[bB][i] % 8;
                //up left
                for(int j = 1; j <= 7; j++)
                {
                    if(index_x - j < 0 || index_y - j < 0)
                    {
                        break;
                    }
                    if(pos->board[index_x - j][index_y - j] == ' ' || pos->board[index_x - j][index_y - j] == 'P' || pos->board[index_x - j][index_y - j] == 'R' || pos->board[index_x - j][index_y - j] == 'N' || pos->board[index_x - j][index_y - j] == 'B' || pos->board[index_x - j][index_y - j] == 'Q')  
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x - j][index_y - j], 3);
                        index++;
                    }
                    if(pos->board[index_x - j][index_y - j] != ' ')
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
                    if(pos->board[index_x - j][index_y + j] == ' ' || pos->board[index_x - j][index_y + j] == 'P' || pos->board[index_x - j][index_y + j] == 'R' || pos->board[index_x - j][index_y + j] == 'N' || pos->board[index_x - j][index_y + j] == 'B' || pos->board[index_x - j][index_y + j] == 'Q') 
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x - j][index_y + j], 3);
                        index++;
                    }
                    if(pos->board[index_x - j][index_y + j] != ' ')
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
                    if(pos->board[index_x + j][index_y - j] == ' ' || pos->board[index_x + j][index_y - j] == 'P' || pos->board[index_x + j][index_y - j] == 'R' || pos->board[index_x + j][index_y - j] == 'N' || pos->board[index_x + j][index_y - j] == 'B' || pos->board[index_x + j][index_y - j] == 'Q')    
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x + j][index_y - j], 3);
                        index++;
                    }
                    if(pos->board[index_x + j][index_y - j] != ' ')
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
                    if(pos->board[index_x + j][index_y + j] == ' ' || pos->board[index_x + j][index_y + j] == 'P' || pos->board[index_x + j][index_y + j] == 'R' || pos->board[index_x + j][index_y + j] == 'N' || pos->board[index_x + j][index_y + j] == 'B' || pos->board[index_x + j][index_y + j] == 'Q')   
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x + j][index_y + j], 3);
                        index++;
                    }
                    if(pos->board[index_x + j][index_y + j] != ' ')
                    {
                        break;
                    }
                }
            }
            //queen
            for(int i = 0; i < pos->piece_count[bQ]; i++)
            {
                index_x = pos->piece_list[bQ][i] / 8;
                index_y = pos->piece_list[bQ][i] % 8;
                //up left
                for(int j = 1; j <= 7; j++)
                {
                    if(index_x - j < 0 || index_y - j < 0)
                    {
                        break;
                    }
                    if(pos->board[index_x - j][index_y - j] == ' ' || pos->board[index_x - j][index_y - j] == 'P' || pos->board[index_x - j][index_y - j] == 'R' || pos->board[index_x - j][index_y - j] == 'N' || pos->board[index_x - j][index_y - j] == 'B' || pos->board[index_x - j][index_y - j] == 'Q')  
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x - j][index_y - j], 3);
                        index++;
                    }
                    if(pos->board[index_x - j][index_y - j] != ' ')
                    {
                        break;
                    }
                }
                //up
                for(int j = index_x-1; j >= 0; j--)
                {
                    if(pos->board[j][index_y] == ' ' || pos->board[j][index_y] == 'P' || pos->board[j][index_y] == 'R' || pos->board[j][index_y] == 'N' || pos->board[j][index_y] == 'B' || pos->board[j][index_y] == 'Q') 
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
                        index++;
                    }
                    if(pos->board[j][index_y] != ' ')
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
                    if(pos->board[index_x - j][index_y + j] == ' ' || pos->board[index_x - j][index_y + j] == 'P' || pos->board[index_x - j][index_y + j] == 'R' || pos->board[index_x - j][index_y + j] == 'N' || pos->board[index_x - j][index_y + j] == 'B' || pos->board[index_x - j][index_y + j] == 'Q') 
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x - j][index_y + j], 3);
                        index++;
                    }
                    if(pos->board[index_x - j][index_y + j] != ' ')
                    {
                        break;
                    }
                }
                //left
                for(int j = index_y-1; j >= 0; j--)
                {
                    if(pos->board[index_x][j] == ' ' || pos->board[index_x][j] == 'P' || pos->board[index_x][j] == 'R' || pos->board[index_x][j] == 'N' || pos->board[index_x][j] == 'B' || pos->board[index_x][j] == 'Q')
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
                        index++;
                    }
                    if(pos->board[index_x][j] != ' ')
                    {
                        break;
                    }
                }
                //right
                for(int j = index_y+1; j <= 7; j++)
                {
                    if(pos->board[index_x][j] == ' ' || pos->board[index_x][j] == 'P' || pos->board[index_x][j] == 'R' || pos->board[index_x][j] == 'N' || pos->board[index_x][j] == 'B' || pos->board[index_x][j] == 'Q') 
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
                        index++;
                    }
                    if(pos->board[index_x][j] != ' ')
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
                    if(pos->board[index_x + j][index_y - j] == ' ' || pos->board[index_x + j][index_y - j] == 'P' || pos->board[index_x + j][index_y - j] == 'R' || pos->board[index_x + j][index_y - j] == 'N' || pos->board[index_x + j][index_y - j] == 'B' || pos->board[index_x + j][index_y - j] == 'Q')  
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x + j][index_y - j], 3);
                        index++;
                    }
                    if(pos->board[index_x + j][index_y - j] != ' ')
                    {
                        break;
                    }
                }
                //down
                for(int j = index_x+1; j <= 7; j++)
                {
                    if(pos->board[j][index_y] == ' ' || pos->board[j][index_y] == 'P' || pos->board[j][index_y] == 'R' || pos->board[j][index_y] == 'N' || pos->board[j][index_y] == 'B' || pos->board[j][index_y] == 'Q') 
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
                        index++;
                    }
                    if(pos->board[j][index_y] != ' ')
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
                    if(pos->board[index_x + j][index_y + j] == ' ' || pos->board[index_x + j][index_y + j] == 'P' || pos->board[index_x + j][index_y + j] == 'R' || pos->board[index_x + j][index_y + j] == 'N' || pos->board[index_x + j][index_y + j] == 'B' || pos->board[index_x + j][index_y + j] == 'Q')   
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x + j][index_y + j], 3);
                        index++;
                    }
                    if(pos->board[index_x + j][index_y + j] != ' ')
                    {
                        break;
                    }
                }
            }
            break;
        }
        //white pieces
        case -1:
        {
            //pawn
            for(int i = 0; i < pos->piece_count[wP]; i++)
            {
                index_x = pos->piece_list[wP][i] / 8;
                index_y = pos->piece_list[wP][i] % 8;
                //2 steps at starting position
                if(index_x == 6)
                {
                    if(pos->board[5][index_y] == ' ' && pos->board[4][index_y] == ' ')  
                    { 
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[4][index_y], 3);
                        index++;
                    }
                }
                for(y = index_y - 1; y < index_y + 2; y++)
                {
                    if(y & 8) //skip when out of board
                    {
                        continue;
                    }
                    if(CheckMove_wpawn(pos, index_y, index_x - 1, y)) 
                    { 
                        //promotions
                        if((index_x - 1) == 0)
                        {
                            strncpy(all_moves[index], positions[index_x][index_y], 3);
                            strncat(all_moves[index], positions[index_x - 1][y], 3);
                            all_moves[index][4] = 'q';
                            all_moves[index][5] = '\0';
                            index++;
                            strncpy(all_moves[index], all_moves[index-1], 6);
                            all_moves[index][4] = 'r';
                            index++;
                            strncpy(all_moves[index], all_moves[index-1], 6);
                            all_moves[index][4] = 'b';
                            index++;
                            strncpy(all_moves[index], all_moves[index-1], 6);
                            all_moves[index][4] = 'n';
                            index++;
                        }
                        else
                        {
                            strncpy(all_moves[index], positions[index_x][index_y], 3);
                            strncat(all_moves[index], positions[index_x - 1][y], 3);
                            index++;
                        }
                    }
                }
            }
            //knight
            for(int i = 0; i < pos->piece_count[wN]; i++)
            {
                index_x = pos->piece_list[wN][i] / 8;
                index_y = pos->piece_list[wN][i] % 8;
                for(int j = 0; j < 8; j++)
                {
                    x = index_x + knight_moves_x[j];
                    y = index_y + knight_moves_y[j];
                    if(x & 8 || y & 8) //skip when out of board
                    {
                        continue;
                    }
                    if(pos->board[x][y] == ' ' || pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')   
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[x][y], 3);
                        index++;
                    }
                }
            }
            //king
            index_x = pos->piece_list[wK][0] / 8;
            index_y = pos->piece_list[wK][0] % 8;
            //castling
            if(index_x == 7 && index_y == 4)
            {
                if(CheckMove_wkingside(pos))  
                {  
                    strncpy(all_moves[index], positions[index_x][index_y], 3);
                    strncat(all_moves[index], positions[7][6], 3);
                    index++;
                }
                if(CheckMove_wqueenside(pos))  
                {  
                    strncpy(all_moves[index], positions[index_x][index_y], 3);
                    strncat(all_moves[index], positions[7][2], 3);
                    index++;
                }
            }
            for(int j = 0; j < 8; j++)
            {
                x = index_x + king_moves_x[j];
                y = index_y + king_moves_y[j];
                if(x & 8 || y & 8) //skip when out of board
                {
                    continue;
                }
                if(pos->board[x][y] == ' ' || pos->board[x][y] == 'p' || pos->board[x][y] == 'r' || pos->board[x][y] == 'n' || pos->board[x][y] == 'b' || pos->board[x][y] == 'q')   
                {  
                    strncpy(all_moves[index], positions[index_x][index_y], 3);
                    strncat(all_moves[index], positions[x][y], 3);
                    index++;
                }
            }
            //rook
            for(int i = 0; i < pos->piece_count[wR]; i++)
            {
                index_x = pos->piece_list[wR][i] / 8;
                index_y = pos->piece_list[wR][i] % 8;
                //up
                for(int j = index_x-1; j >= 0; j--)
                {
                    if(pos->board[j][index_y] == ' ' || pos->board[j][index_y] == 'p' || pos->board[j][index_y] == 'r' || pos->board[j][index_y] == 'n' || pos->board[j][index_y] == 'b' || pos->board[j][index_y] == 'q') 
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
                        index++;
                    }
                    if(pos->board[j][index_y] != ' ')
                    {
                        break;
                    }
                }
                //left
                for(int j = index_y-1; j >= 0; j--)
                {
                    if(pos->board[index_x][j] == ' ' || pos->board[index_x][j] == 'p' || pos->board[index_x][j] == 'r' || pos->board[index_x][j] == 'n' || pos->board[index_x][j] == 'b' || pos->board[index_x][j] == 'q') 
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
                        index++;
                    }
                    if(pos->board[index_x][j] != ' ')
                    {
                        break;
                    }
                }
                //right
                for(int j = index_y+1; j <= 7; j++)
                {
                    if(pos->board[index_x][j] == ' ' || pos->board[index_x][j] == 'p' || pos->board[index_x][j] == 'r' || pos->board[index_x][j] == 'n' || pos->board[index_x][j] == 'b' || pos->board[index_x][j] == 'q')  
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
                        index++;
                    }
                    if(pos->board[index_x][j] != ' ')
                    {
                        break;
                    }
                }
                //down
                for(int j = index_x+1; j <= 7; j++)
                {
                    if(pos->board[j][index_y] == ' ' || pos->board[j][index_y] == 'p' || pos->board[j][index_y] == 'r' || pos->board[j][index_y] == 'n' || pos->board[j][index_y] == 'b' || pos->board[j][index_y] == 'q') 
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
                        index++;
                    }
                    if(pos->board[j][index_y] != ' ')
                    {
                        break;
                    }
                }
            }
            //bishop
            for(int i = 0; i < pos->piece_count[wB]; i++)
            {
                index_x = pos->piece_list[wB][i] / 8;
                index_y = pos->piece_list[wB][i] % 8;
                //up left
                for(int j = 1; j <= 7; j++)
                {
                    if(index_x - j < 0 || index_y - j < 0)
                    {
                        break;
                    }
                    if(pos->board[index_x - j][index_y - j] == ' ' || pos->board[index_x - j][index_y - j] == 'p' || pos->board[index_x - j][index_y - j] == 'r' || pos->board[index_x - j][index_y - j] == 'n' || pos->board[index_x - j][index_y - j] == 'b' || pos->board[index_x - j][index_y - j] == 'q')  
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x - j][index_y - j], 3);
                        index++;
                    }
                    if(pos->board[index_x - j][index_y - j] != ' ')
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
                    if(pos->board[index_x - j][index_y + j] == ' ' || pos->board[index_x - j][index_y + j] == 'p' || pos->board[index_x - j][index_y + j] == 'r' || pos->board[index_x - j][index_y + j] == 'n' || pos->board[index_x - j][index_y + j] == 'b' || pos->board[index_x - j][index_y + j] == 'q')   
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x - j][index_y + j], 3);
                        index++;
                    }
                    if(pos->board[index_x - j][index_y + j] != ' ')
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
                    if(pos->board[index_x + j][index_y - j] == ' ' || pos->board[index_x + j][index_y - j] == 'p' || pos->board[index_x + j][index_y - j] == 'r' || pos->board[index_x + j][index_y - j] == 'n' || pos->board[index_x + j][index_y - j] == 'b' || pos->board[index_x + j][index_y - j] == 'q')   
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x + j][index_y - j], 3);
                        index++;
                    }
                    if(pos->board[index_x + j][index_y - j] != ' ')
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
                    if(pos->board[index_x + j][index_y + j] == ' ' || pos->board[index_x + j][index_y + j] == 'p' || pos->board[index_x + j][index_y + j] == 'r' || pos->board[index_x + j][index_y + j] == 'n' || pos->board[index_x + j][index_y + j] == 'b' || pos->board[index_x + j][index_y + j] == 'q')   
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x + j][index_y + j], 3);
                        index++;
                    }
                    if(pos->board[index_x + j][index_y + j] != ' ')
                    {
                        break;
                    }
                }
            }
            //queen
            for(int i = 0; i < pos->piece_count[wQ]; i++)
            {
                index_x = pos->piece_list[wQ][i] / 8;
                index_y = pos->piece_list[wQ][i] % 8;
                //up left
                for(int j = 1; j <= 7; j++)
                {
                    if(index_x - j < 0 || index_y - j < 0)
                    {
                        break;
                    }
                    if(pos->board[index_x - j][index_y - j] == ' ' || pos->board[index_x - j][index_y - j] == 'p' || pos->board[index_x - j][index_y - j] == 'r' || pos->board[index_x - j][index_y - j] == 'n' || pos->board[index_x - j][index_y - j] == 'b' || pos->board[index_x - j][index_y - j] == 'q')  
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x - j][index_y - j], 3);
                        index++;
                    }
                    if(pos->board[index_x - j][index_y - j] != ' ')
                    {
                        break;
                    }
                }
                //up
                for(int j = index_x-1; j >= 0; j--)
                {
                    if(pos->board[j][index_y] == ' ' || pos->board[j][index_y] == 'p' || pos->board[j][index_y] == 'r' || pos->board[j][index_y] == 'n' || pos->board[j][index_y] == 'b' || pos->board[j][index_y] == 'q')  
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
                        index++;
                    }
                    if(pos->board[j][index_y] != ' ')
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
                    if(pos->board[index_x - j][index_y + j] == ' ' || pos->board[index_x - j][index_y + j] == 'p' || pos->board[index_x - j][index_y + j] == 'r' || pos->board[index_x - j][index_y + j] == 'n' || pos->board[index_x - j][index_y + j] == 'b' || pos->board[index_x - j][index_y + j] == 'q')   
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x - j][index_y + j], 3);
                        index++;
                    }
                    if(pos->board[index_x - j][index_y + j] != ' ')
                    {
                        break;
                    }
                }
                //left
                for(int j = index_y-1; j >= 0; j--)
                {
                    if(pos->board[index_x][j] == ' ' || pos->board[index_x][j] == 'p' || pos->board[index_x][j] == 'r' || pos->board[index_x][j] == 'n' || pos->board[index_x][j] == 'b' || pos->board[index_x][j] == 'q')   
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
                        index++;
                    }
                    if(pos->board[index_x][j] != ' ')
                    {
                        break;
                    }
                }
                //right
                for(int j = index_y+1; j <= 7; j++)
                {
                    if(pos->board[index_x][j] == ' ' || pos->board[index_x][j] == 'p' || pos->board[index_x][j] == 'r' || pos->board[index_x][j] == 'n' || pos->board[index_x][j] == 'b' || pos->board[index_x][j] == 'q')   
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x][j], 3);
                        index++;
                    }
                    if(pos->board[index_x][j] != ' ')
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
                    if(pos->board[index_x + j][index_y - j] == ' ' || pos->board[index_x + j][index_y - j] == 'p' || pos->board[index_x + j][index_y - j] == 'r' || pos->board[index_x + j][index_y - j] == 'n' || pos->board[index_x + j][index_y - j] == 'b' || pos->board[index_x + j][index_y - j] == 'q')   
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x + j][index_y - j], 3);
                        index++;
                    }
                    if(pos->board[index_x + j][index_y - j] != ' ')
                    {
                        break;
                    }
                }
                //down
                for(int j = index_x+1; j <= 7; j++)
                {
                    if(pos->board[j][index_y] == ' ' || pos->board[j][index_y] == 'p' || pos->board[j][index_y] == 'r' || pos->board[j][index_y] == 'n' || pos->board[j][index_y] == 'b' || pos->board[j][index_y] == 'q') 
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[j][index_y], 3);
                        index++;
                    }
                    if(pos->board[j][index_y] != ' ')
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
                    if(pos->board[index_x + j][index_y + j] == ' ' || pos->board[index_x + j][index_y + j] == 'p' || pos->board[index_x + j][index_y + j] == 'r' || pos->board[index_x + j][index_y + j] == 'n' || pos->board[index_x + j][index_y + j] == 'b' || pos->board[index_x + j][index_y + j] == 'q')   
                    {  
                        strncpy(all_moves[index], positions[index_x][index_y], 3);
                        strncat(all_moves[index], positions[index_x + j][index_y + j], 3);
                        index++;
                    }
                    if(pos->board[index_x + j][index_y + j] != ' ')
                    {
                        break;
                    }
                }
            }
            break;
        }
    }
    
    return index;
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
unsigned long long int getHash(BOARD *pos, int color)
{
    unsigned long long int h = 0;
    int chess_piece;
    for(int x = 0; x < 8; x++)
    {
        for(int y = 0; y < 8; y++)
        {
            if(pos->board[x][y] != ' ')
            {
                chess_piece = piece_code(pos->board[x][y]);
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
    if(pos->ksw)
    {
        h ^= kswcr;
    }
    if(pos->qsw)
    {
        h ^= qswcr;
    }
    if(pos->ksb)
    {
        h ^= ksbcr;
    }
    if(pos->qsb)
    {
        h ^= qsbcr;
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
    
    return h;
}

static unsigned long long int perft(BOARD *pos, int depth, int color)
{   
    unsigned long long int nodes = 0;
    int length;
    char cp[3], np[3];
    char promotion;
    BOARD pos_copy;

    if(depth == 0)
        return 1ULL;
    
    //get children of node
    char moves[256][6];
    length = moveGen(pos, moves, color);
    
    for(int x = 0; x < length; x++)
    {
        //make a copy of the board
        pos_copy = *pos;
        sscanf(moves[x], "%2s%2s%c", cp, np, &promotion);
        //make move
        makeMove(cp, np, promotion, &pos_copy);
        
        //check if check is ignored
        if(ifCheck(&pos_copy, color))
        {
            continue;
        }
        
        nodes += perft(&pos_copy, depth - 1, -color);
    }
    return nodes;
}

void init_board(BOARD *pos)
{
    memcpy(pos->board, board2, sizeof(pos->board));
    pos->ksb = 1;
    pos->qsb = 1;
    pos->ksw = 1;
    pos->qsw = 1;
    pos->ep_file = 0;
    pos->halfmove_counter = 0;
    pos->piece_num = 32;
    memcpy(pos->piece_list, piece_list2, sizeof(pos->piece_list));
    memcpy(pos->piece_count, piece_count2, sizeof(pos->piece_count));
    memcpy(pos->index_board, index_board2, sizeof(pos->index_board));
    pos->key = getHash(pos, -1);
}

int main()
{
    init_zobrist();
    BOARD pos;
    init_board(&pos);

    int nodes = perft(&pos, 5, -1);

    printf("nodes count: %lld\n", nodes);
    return 0;
}