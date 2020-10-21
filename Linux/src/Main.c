#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include "Board.h"
#include "Move.h"
#include "Search.h"
#include "Transposition.h"

#define VERSION "2.1.4"

//global variables
//specify color for engine
//1: black
//-1: white
int engine_color;
int outofbook;
BOARD pos;
char op_move[6] = "";
bool newgame;

void * engine(void * param)
{
    search(&pos, engine_color, op_move);

    pthread_exit(NULL);
    return 0;
}

void handle_uci()
{
    printf("id name Supernova %s\n", VERSION);
    printf("id author Minkai Yang\n");
    //options
    printf("option name Hash type spin default 32 min 1 max 4096\n");
    printf("option name Ponder type check default false\n");
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
    else if(hash_size > 4096)
        hash_size = 4096;
    HASHSIZE = (unsigned long int)((1048576.0 / sizeof(struct DataItem)) * (3.0 * hash_size / 4.0));
    tt = malloc(HASHSIZE * sizeof(struct DataItem));
    EVALHASHSIZE = (unsigned long int)((1048576.0 / sizeof(struct Eval)) * (hash_size / 4.0));
    Evaltt = malloc(EVALHASHSIZE * sizeof(struct Eval));
    clearTT();
    clearEvalTT();
}

//some GUI might not support this command
void handle_newgame()
{
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
    //generate random zobrist numbers
    init_zobrist();
    memset(history, 0, sizeof(history)); //clear history heuristic table
    outofbook = 0;
    //clear hash tables
    clearTT();
    clearEvalTT();
    clearPawnTT();
    newgame = true;
}

//parse fen notation
void parse_fen(char *position, BOARD *pos)
{
    char halfmove[20] = "";
    char fullmove[20] = "";
    int piece_count = 0;
    int x = 0, y = 0;
    clear_board(pos->board);
    position+=4;
    //loop through fen
    while(*position && x < 8)
    {
        switch(*position)
        {
            case 'p':
                pos->board[x][y] = 'p';
                y++;
                piece_count++;
                break;
            case 'n':
                pos->board[x][y] = 'n';
                y++;
                piece_count++;
                break;
            case 'b':
                pos->board[x][y] = 'b';
                y++;
                piece_count++;
                break;
            case 'r':
                pos->board[x][y] = 'r';
                y++;
                piece_count++;
                break;
            case 'q':
                pos->board[x][y] = 'q';
                y++;
                piece_count++;
                break;
            case 'k':
                pos->board[x][y] = 'k';
                y++;
                piece_count++;
                break;
            case 'P':
                pos->board[x][y] = 'P';
                y++;
                piece_count++;
                break;
            case 'N':
                pos->board[x][y] = 'N';
                y++;
                piece_count++;
                break;
            case 'B':
                pos->board[x][y] = 'B';
                y++;
                piece_count++;
                break;
            case 'R':
                pos->board[x][y] = 'R';
                y++;
                piece_count++;
                break;
            case 'Q':
                pos->board[x][y] = 'Q';
                y++;
                piece_count++;
                break;
            case 'K':
                pos->board[x][y] = 'K';
                y++;
                piece_count++;
                break;
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
                y += *position - '0';
                break;
            case '/':
                x++;
                y = 0;
                break;
            case ' ':
                x++;
                break;
            default:
                printf("Can't parse fen!\n");
                exit(0);
                break;
        }
        position++;
    }
    //save number of pieces
    pos->piece_num = piece_count;
    //side to move
    if(*position == 'w')
        engine_color = -1;
    else if(*position == 'b')
        engine_color = 1;
    //castling rights
    position+=2;
    pos->ksb = 0;
    pos->qsb = 0;
    pos->ksw = 0;
    pos->qsw = 0;
    for(int i = 0; i < 4; i++)
    {
        switch(*position)
        {
            case 'K':
                pos->ksw = 1;
                break;
            case 'Q':
                pos->qsw = 1;
                break;
            case 'k':
                pos->ksb = 1;
                break;
            case 'q':
                pos->qsb = 1;
                break;
            case '-':
                break;
        }
        position++;
        if(*position == ' ')
            break;
    }
    //en passant
    position++;
    pos->ep_file = 0;
    if(*position != '-')
    {
        pos->ep_file = *position - 96;
        position++;
    }
    //half move counter
    position+=2;
    sscanf(position, "%s %s", halfmove, fullmove);
    pos->halfmove_counter = atoi(halfmove);
    history_index = pos->halfmove_counter;
    set_piecelists(pos);
    pos->key = getHash(pos, engine_color);
    pos->pawn_key = getPawnHash(pos->board);
    pos->pawn_push = false;
}

void handle_position(char *input)
{
    char move[6] = "";
    MOVE smove;
    char own_piece = ' ';
    char op_piece = ' ';
    //parse the positon input
    char *position;
    char *move_str;
    const char s[2] = " ";
    memset(op_move, 0, sizeof(op_move));
    memset(history_log, -1, sizeof(history_log)); //clear history table

    //in case GUI doesn't send ucinewgame command
    if(!newgame)
    {
        if(tt == NULL) //default tt if not set
        {
            HASHSIZE = (unsigned long int)((1048576.0 / sizeof(struct DataItem)) * 24);
            tt = malloc(HASHSIZE * sizeof(struct DataItem));
            clearTT();
        }
        if(Evaltt == NULL) //default tt if not set
        {
            EVALHASHSIZE = (unsigned long int)((1048576.0 / sizeof(struct Eval)) * 8);
            Evaltt = malloc(EVALHASHSIZE * sizeof(struct Eval));
            clearEvalTT();
        }
        init_zobrist();
        memset(history, 0, sizeof(history)); //clear history heuristic table
        clearPawnTT();
        outofbook = 0;
        newgame = true;
    }

    //parse start position
    if((position = strstr(input, "startpos")))
    {
        //set the board struct to initial state
        init_board(&pos);
        history_log[0] = pos.key; //get the initial hash key
        history_index = 0;
        engine_color = -1; //white
    }
    //parse fen notation
    else if((position = strstr(input, "fen")))
    {
        parse_fen(position, &pos);
    }

    //parse moves
    if((position = strstr(input, "moves")))
    {
        move_str = strtok(position, s);
        while(move_str != NULL) 
        {
            move_str = strtok(NULL, s);
            if(move_str != NULL)
            {
                memcpy(move, move_str, 6);
                move[5] = '\0';
                //convert move string to move struct
                smove = string_to_move(move);
                //set the board at the start of the game
                own_piece = pos.board[smove.from / 8][smove.from % 8];
                op_piece = pos.board[smove.to / 8][smove.to % 8];
                makeMove(&pos, &smove);
                
                //store board into move history
                history_index++;
                history_log[history_index] = pos.key;
            }
        }
        //get opponent's move
        //only when it's a capture
        if(op_piece != ' ')
        {
            strncpy(op_move, move, 6);
            op_move[5] = '\0';
        }
        //check what color does the gui wants the engine to be
        if(islower(own_piece))
        {
            engine_color = -1; //white
        }
        else
        {
            engine_color = 1; //black
        }
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
    ponder_time = __DBL_MAX__;
    extra_time = true;
    analyze = false;
    node_mode = false;
    time_management = false;
    sscanf(input, "go %s %s", option, buffer); //get the go command
    if(!strncmp("wtime", option, 5))
    {	
        //get remaining time on the clock
        sscanf(input, "go wtime %s btime %s winc %s binc %s movestogo %s", white_time, black_time, white_inc, black_inc, movestogo);
        wt = (double)atoi(white_time) / 1000; //convert to sec
        bt = (double)atoi(black_time) / 1000;
        if(strncmp("", white_inc, 19))
        {
            winc = (double)atoi(white_inc) / 1000;
        }
        if(strncmp("", black_inc, 19))
        {
            binc = (double)atoi(black_inc) / 1000;
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
            if((1.75 * search_time) >= wt) //prevent losing on time
                extra_time = false;
        }
        else if(engine_color == 1)
        {
            search_time = bt / moves_left + 0.9 * binc;
            if((1.75 * search_time) >= bt) //prevent losing on time
                extra_time = false;
        }
        if(outofbook < 5) //think longer when out of book
        {
            search_time *= 1.2;
            outofbook++;
        }
        time_management = true;
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
        }
        if(strncmp("", black_inc, 19))
        {
            binc = (double)atoi(black_inc) / 1000;
        }
        if(strncmp("", movestogo, 19))
        {
            moves_left = atoi(movestogo) + 1;
        }
        if(engine_color == -1)
        {
            ponder_time = wt / moves_left + 0.9 * winc;
            if((1.75 * ponder_time) >= wt) //prevent losing on time
                extra_time = false;
        }
        else if(engine_color == 1)
        {
            ponder_time = bt / moves_left + 0.9 * binc;
            if((1.75 * ponder_time) >= bt) //prevent losing on time
                extra_time = false;
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
        //prevent depth > MAXDETPTH
        if(search_depth > MAXDEPTH)
            search_depth = MAXDEPTH;
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
    char string[4000];
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    //infinite loop for uci gui
    while(true)
    {
        memset(&string[0], 0, sizeof(string)); //flush the string
        fflush(stdout); //flush the stdout
        if(!fgets(string, 4000, stdin))
        {
            continue;
        }
        //skip new line
        if(string[0] == '\n')
        {
            continue;
        }
        //listen for command
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
            clearPawnTT();
            memset(history, 0, sizeof(history)); //clear history heuristic table
        }
        else if(!strncmp("quit", string, 4))
        {
            stop = true;
            sleep(0.3); //wait till all threads are done
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
    newgame = false;
    uci_loop();
    return 0;
}