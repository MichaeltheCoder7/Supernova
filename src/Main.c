/* Main.c */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <pthread.h>
#include <windows.h>
#include "Board.h"
#include "Search.h"
#include "Evaluate.h"


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


//global variables for uci engine
//specify color for engine
//1: black
//-1: white
int engine_color = 0;
char opponent_move[5] = "";
int kswflag = 1;
int qswflag = 1;
int ksbflag = 1;
int qsbflag = 1;
int halfmove_counter = 0;
int outofbook;


void * engine(void * param)
{
	char opponentCP[3] = "", opponentNP[3] = "";

	if(strncmp("", opponent_move, 5))
	{
		sscanf(opponent_move, "%2s%2s", opponentCP, opponentNP);
	}

	//displayboard(board);

	search(board, engine_color, opponentCP, opponentNP, kswflag, qswflag, ksbflag, qsbflag, halfmove_counter);

	pthread_exit(NULL);
	return 0;
}

void handle_uci()
{
	printf("id name Supernova 1.0\n");
	printf("id author Minkai Yang\n");
	printf("uciok\n");
}

void handle_newgame()
{
	//start the game
	strncpy(opponent_move, "", 5);
	engine_color = 0;
	init_zobrist();
	memset(history, 0, sizeof(history)); //clear history heuristic table
	kswflag = 1;
	qswflag = 1;
	ksbflag = 1;
	qsbflag = 1; //reset the castling flags
	clearTT(false);
	clearEvalTT(false);
	outofbook = 0;
}

void handle_position(char *input)
{
	char move[5];
	char cp[3];
	char np[3];
	char own_piece = ' ';
	char op_piece;
	int move_color = 0;
	char promotion_piece = ' ';
	//parse the positon input
	char *position;
	const char s[2] = " ";
	memset(history_log, -1, sizeof(history_log)); //clear history table
	position = strtok(input, s);

	halfmove_counter = 0;
	resetboard(board); //reset the board
	history_log[0] = getHash(board, -1, "", "", 1, 1, 1, 1);
	history_index = 1;

	while(position != NULL) 
	{
		position = strtok(NULL, s);
		if(position != NULL)
		{
			strncpy(move, position, 4); //get opponent's move
			move[4] = '\0';
			promotion_piece = position[4];
			if(strncmp("star", move, 4) != 0 && strncmp("move", move, 4) != 0) //set the board at the start of the game
			{
				sscanf(move, "%2s%2s", cp, np);
				own_piece = position_to_piece(board, cp);
				op_piece = position_to_piece(board, np);
				makeMove(cp, np, promotion_piece, own_piece, op_piece, board);

				//Check if kings and rooks have moved for castling
				if(board[7][4] != 'K')
				{
					kswflag = qswflag = 0;
				}
				if(board[0][4] != 'k')
				{
					ksbflag = qsbflag = 0;
				}
				if(board[7][0] != 'R')
				{
					qswflag = 0;
				}
				if(board[7][7] != 'R')
				{
					kswflag = 0;
				}
				if(board[0][0] != 'r')
				{
					qsbflag = 0;
				}
				if(board[0][7] != 'r')
				{
					ksbflag = 0;
				}
				
				//fifty-move rule counter
				if(op_piece == ' ' && own_piece != 'P' && own_piece != 'p')
				{
					halfmove_counter++;
				}
				else
				{
					halfmove_counter = 0;
				}

				if(islower(own_piece))
				{
					move_color = -1; //white
				}
				else
				{
					move_color = 1; //black
				}
				
				//store board into move history
				history_log[history_index] = getHash(board, move_color, cp, np, kswflag, qswflag, ksbflag, qsbflag);
				history_index++;
			}
		}
	}

	//check what color does the gui wants the engine to be
	if(!strncmp("star", move, 4) || islower(own_piece))
	{
		engine_color = -1; //white
	}
	else
	{
		engine_color = 1; //black
	}

	if(strncmp("star", move, 4) != 0)
	{
		strncpy(opponent_move, move, 4);
		opponent_move[4] = '\0';
	}
}   

void handle_go(char *input)
{
	char option[10] = "";
	char white_time[20] = "";
	char black_time[20] = "";
	double wt;
	double bt;
	sscanf(input, "go %s %s btime %s winc 0 binc 0\n", option, white_time, black_time); //get remaining time on the clock
	if(!strncmp("infinite", option, 8))
	{
		search_time = __DBL_MAX__;
		ponder = false;
		analyze = true;
	}
	else if(!strncmp("ponder", option, 6))
	{
		search_time = __DBL_MAX__;
		sscanf(input, "go ponder wtime %s btime %s winc 0 binc 0\n", white_time, black_time); //get remaining time on the clock
		wt = (double)atoi(white_time) / 1000;	//convert to sec
		bt = (double)atoi(black_time) / 1000;
		//sudden death time control
		if(engine_color == -1)
		{
			ponder_time = wt / 30;	//assume 30 moves are left
		}
		else if(engine_color == 1)
		{
			ponder_time = bt / 30;
		}
		ponder = true;
		analyze = false;
	}
	else
	{	
		wt = (double)atoi(white_time) / 1000;	//convert to sec
		bt = (double)atoi(black_time) / 1000;
		//sudden death time control
		if(engine_color == -1)
		{
			search_time = wt / 30;	//assume 30 moves are left
		}
		else if(engine_color == 1)
		{
			search_time = bt / 30;
		}
		if(outofbook < 5) //think longer when out of book
		{
			search_time *= 1.2;
			outofbook++;
		}
		ponder = false;
		analyze = false;
	}

	stop = false; //set stop to false
	ponderhit = false;
	//start engine thread
	pthread_attr_t tattr;
	pthread_t thread;
	int error;

	pthread_attr_init(&tattr);
	pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
	error = pthread_create(&thread, &tattr, engine, NULL);

	if(error)
	{
		printf("An error occured: %d\n", error);
		exit(0);
	}
}

void uci_loop()
{
	setbuf(stdin, NULL);
	setbuf(stdout, NULL);
	char string[3000];
	//infinite loop for uci gui
	while(true)
	{
		memset(&string[0], 0, sizeof(string)); //flush the string
		fflush(stdout); //flush the stdout
		if(!fgets(string, 3000, stdin))
		{
			continue;
		}

		if(string[0] == '\n')
		{
			continue;
		}

		if(!strncmp("isready", string, 7))
		{
			printf("readyok\n");
		}
		else if(!strncmp("ucinewgame", string, 10))
		{
			handle_newgame();
		}
		else if(!strncmp("position", string, 8))
		{
			handle_position(string);
		}
		else if(!strncmp("go", string, 2))
		{
			handle_go(string);
		}
		else if(!strncmp("uci", string, 3))
		{
			handle_uci();
		}
		else if(!strncmp("stop", string, 4))
		{
			stop = true;
		}
		else if(!strncmp("ponderhit", string, 9))
		{
			ponderhit = true;
		}
		else if(!strncmp("quit", string, 4))
		{
			stop = true;
			Sleep(300); //wait till all threads are done
			break;
		}
	}
}

//uci gui
int main(void)
{
	uci_loop();
	return 0;
}

