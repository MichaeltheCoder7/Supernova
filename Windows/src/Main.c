#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <pthread.h>
#include <windows.h>
#include <assert.h>
#include "Board.h"
#include "Search.h"
#include "Transposition.h"

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

int pieceList[12][10] = {

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

int pieceCount[12] = { 8, 2, 2, 2, 1, 1, 8, 2, 2, 2, 1, 1 };

int indexBoard[64] = {
	  0,   0,   0,   0,   0,   1,   1,   1,
	  0,   1,   2,   3,   4,   5,   6,   7,
	 -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
	 -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
	 -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
	 -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
	  0,   1,   2,   3,   4,   5,   6,   7,
	  0,   0,   0,   0,   0,   1,   1,   1
};

//global variables
//specify color for engine
//1: black
//-1: white
int engine_color;
int outofbook;
BOARD pos;

void * engine(void * param)
{
	search(&pos, engine_color);

	pthread_exit(NULL);
	return 0;
}

void handle_uci()
{
	printf("id name Supernova 2.0\n");
	printf("id author Minkai Yang\n");
	//options
	printf("option name Hash type spin default 32 min 1 max 2048\n");
	printf("option name Ponder type check default true\n");
	printf("option name Clear Hash type button\n");
	printf("uciok\n");
}

//configure hash table size
void configure_hash(char *input)
{
	char hash_value[10] = "";
	int hash_size;
	
	if(tt != NULL) //free previously allocated tt if any
	{
		free(tt);
		tt = NULL;
	}
	if(Evaltt != NULL) //free previously allocated tt if any
	{
		free(Evaltt);
		Evaltt = NULL;
	}
	sscanf(input, "setoption name Hash value %s\n", hash_value); //get the hash size
	hash_size = atoi(hash_value);
	if(hash_size < 1)
		hash_size = 1;
	else if(hash_size > 2048)
		hash_size = 2048;
	HASHSIZE = (unsigned long int)((1048576.0 / sizeof(struct DataItem)) * (3.0 * hash_size / 4.0));
	tt = malloc(HASHSIZE * sizeof(struct DataItem));
	EVALHASHSIZE = (unsigned long int)((1048576.0 / sizeof(struct Eval)) * (hash_size / 4.0));
	Evaltt = malloc(EVALHASHSIZE * sizeof(struct Eval));
	clearTT();
	clearEvalTT();
}

void handle_newgame()
{
	//start the game
	if(tt == NULL) //default tt if not set
	{
		HASHSIZE = (unsigned long int)((1048576.0 / sizeof(struct DataItem)) * 24);
		tt = malloc(HASHSIZE * sizeof(struct DataItem));
	}
	if(Evaltt == NULL) //default tt if not set
	{
		EVALHASHSIZE = (unsigned long int)((1048576.0 / sizeof(struct Eval)) * 8);
		Evaltt = malloc(EVALHASHSIZE * sizeof(struct Eval));
	}
	engine_color = 0;
	init_zobrist();
	memset(history, 0, sizeof(history)); //clear history heuristic table
	outofbook = 0;
	clearTT();
	clearEvalTT();
}

void init_board(BOARD *pos)
{
    memcpy(pos->board, board, sizeof(pos->board));
	//set the castling flags
    pos->ksb = 1;
    pos->qsb = 1;
    pos->ksw = 1;
    pos->qsw = 1;
	//set en passant files
    pos->ep_file = 0;
    pos->halfmove_counter = 0;
    pos->piece_num = 32;
    memcpy(pos->piece_list, pieceList, sizeof(pos->piece_list));
    memcpy(pos->piece_count, pieceCount, sizeof(pos->piece_count));
    memcpy(pos->index_board, indexBoard, sizeof(pos->index_board));
    pos->key = getHash(pos, -1);
}

void handle_position(char *input)
{
	char move[5];
	char cp[3];
	char np[3];
	char own_piece = ' ';
	char promotion_piece = ' ';
	//parse the positon input
	char *position;
	const char s[2] = " ";
	memset(history_log, -1, sizeof(history_log)); //clear history table
	position = strtok(input, s);

	//set the board struct to initial state
	init_board(&pos);

	history_log[0] = pos.key; //get the initial hash key
	history_index = 0;

	while(position != NULL) 
	{
		position = strtok(NULL, s);
		if(position != NULL)
		{
			strncpy(move, position, 4); //get opponent's move
			move[4] = '\0';
			promotion_piece = position[4];
			//set the board at the start of the game
			if(strncmp("star", move, 4) != 0 && strncmp("move", move, 4) != 0)
			{
				sscanf(move, "%2s%2s", cp, np);
				own_piece = position_to_piece(pos.board, cp);
				makeMove(cp, np, promotion_piece, &pos);
				
				//store board into move history
				history_index++;
				history_log[history_index] = pos.key;
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
}   

void handle_go(char *input)
{
	char option[10] = "";
	char buffer[40] = "";
	char white_time[20] = "";
	char black_time[20] = "";
	char white_inc[20] = "";
	char black_inc[20] = "";
	char movestogo[20] = "";
	double wt;
	double bt;
	double winc = 0, binc = 0;
	int moves_left = 30;
	search_depth = -1;
	ponder_time = 0;
	extra_time = true;
	analyze = false;
	node_mode = false;
	sscanf(input, "go %s %s", option, buffer); //get the go command
	if(!strncmp("wtime", option, 5))
	{	
		//get remaining time on the clock
		sscanf(input, "go wtime %s btime %s winc %s binc %s movestogo %s", white_time, black_time, white_inc, black_inc, movestogo);
		wt = (double)atoi(white_time) / 1000;	//convert to sec
		bt = (double)atoi(black_time) / 1000;
		if(strncmp("", white_inc, 19))
		{
			winc = (double)atoi(white_inc) / 1000;
			if(wt <= 2 * winc) //prevent losing on time
				extra_time = false;
		}
		if(strncmp("", black_inc, 19))
		{
			binc = (double)atoi(black_inc) / 1000;
			if(bt <= 2 * binc)
				extra_time = false;
		}
		if(strncmp("", movestogo, 19))
		{
			//assume one more move to prevent running out of time
			moves_left = atoi(movestogo) + 1; 
		}
		//sudden death time control
		//time = time left / moves to go + 0.9 * increment
		//if moves to go not given, assume 30 moves
		if(engine_color == -1)
		{
			search_time = wt / moves_left + 0.9 * winc;
		}
		else if(engine_color == 1)
		{
			search_time = bt / moves_left + 0.9 * binc;
		}
		if(outofbook < 5) //think longer when out of book
		{
			search_time *= 1.2;
			outofbook++;
		}
	}
	else if(!strncmp("ponder", option, 6))
	{
		search_time = __DBL_MAX__;
		sscanf(input, "go ponder wtime %s btime %s winc %s binc %s movestogo %s", white_time, black_time, white_inc, black_inc, movestogo);
		wt = (double)atoi(white_time) / 1000;
		bt = (double)atoi(black_time) / 1000;
		if(strncmp("", white_inc, 19))
		{
			winc = (double)atoi(white_inc) / 1000;
			if(wt <= 2 * winc)
				extra_time = false;
		}
		if(strncmp("", black_inc, 19))
		{
			binc = (double)atoi(black_inc) / 1000;
			if(bt <= 2 * binc)
				extra_time = false;
		}
		if(strncmp("", movestogo, 19))
		{
			moves_left = atoi(movestogo) + 1;
		}
		if(engine_color == -1)
		{
			ponder_time = wt / moves_left + 0.9 * winc;
		}
		else if(engine_color == 1)
		{
			ponder_time = bt / moves_left + 0.9 * binc;
		}
	}
	else if(!strncmp("movetime", option, 8))
	{
		search_time = (double)atoi(buffer) / 1000;
		extra_time = false;
	}
	else if(!strncmp("depth", option, 5))
	{
		search_time = __DBL_MAX__;
		search_depth = atoi(buffer);
	}
	else if(!strncmp("nodes", option, 5))
	{
		search_time = __DBL_MAX__;
		search_nodes = atoi(buffer);
		node_mode = true;
	}
	else if(!strncmp("infinite", option, 8))
	{
		search_time = __DBL_MAX__;
		analyze = true;
	}
	else
	{
		printf("info string Error! Unknown command!\n");
		search_time = 0;
		search_depth = 0;
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
	char string[4000];
	//infinite loop for uci gui
	while(true)
	{
		memset(&string[0], 0, sizeof(string)); //flush the string
		fflush(stdout); //flush the stdout
		if(!fgets(string, 4000, stdin))
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
		else if(!strncmp("setoption name Hash", string, 19))
		{
			configure_hash(string);
		}
		else if(!strncmp("setoption name Clear Hash", string, 25))
		{
			//clear hash tables 
			clearTT();
			clearEvalTT();
			memset(history, 0, sizeof(history)); //clear history heuristic table
		}
		else if(!strncmp("quit", string, 4))
		{
			stop = true;
			Sleep(300); //wait till all threads are done
			if(tt != NULL)
			{
				free(tt);
				tt = NULL;
			}
			if(Evaltt != NULL)
			{
				free(Evaltt);
				Evaltt = NULL;
			}
			break;
		}
	}
}

//uci gui
int main(void)
{
	tt = NULL; //set hash table pointer to null
	Evaltt = NULL;
	uci_loop();
	return 0;
}