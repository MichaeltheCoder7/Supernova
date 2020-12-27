#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <pthread.h>
#include <assert.h>

#ifdef LINUX
#include <unistd.h>
#else
#include <windows.h>
#endif

#include "Board.h"
#include "Move.h"
#include "Search.h"
#include "Transposition.h"
#include "Syzygy.h"
#include "Fathom/tbprobe.h"

#define VERSION "dev"

// global variables
// specify color for engine
// 1: black
// -1: white
int engine_color;
int outofbook;
BOARD pos_info;
char op_move[6] = "";

void *engine()
{
    search(&pos_info, engine_color, op_move);

    return NULL;
}

void handle_uci()
{
    printf("id name Supernova %s\n", VERSION);
    printf("id author Minkai Yang\n");
    // options
    printf("option name Hash type spin default 32 min 1 max 4096\n");
    printf("option name Clear Hash type button\n");
    printf("option name Ponder type check default false\n");
    printf("option name SyzygyPath type string default <empty>\n");
    printf("option name SyzygyProbeDepth type spin default 1 min 1 max 100\n");
    printf("uciok\n");
    fflush(stdout);
}

// configure hash table size
void configure_hash(char *input)
{
    char hash_value[10] = "";
    int hash_size;

    if (tt != NULL) // free previously allocated main tt if any
    {
        free(tt);
        tt = NULL;
    }
    if (Evaltt != NULL) // free previously allocated eval tt if any
    {
        free(Evaltt);
        Evaltt = NULL;
    }

    sscanf(input, "Hash value %s\n", hash_value); // get the hash size
    hash_size = atoi(hash_value);
    if (hash_size < 1)
        hash_size = 1;
    else if (hash_size > 4096)
        hash_size = 4096;
    
    // allocate 3/4 memory to main hash table and 1/4 memory to eval hash table 
    HASHSIZE = (long)((1048576.0 / sizeof(struct DataItem)) * (3.0 * hash_size / 4.0));
    tt = malloc(HASHSIZE * sizeof(struct DataItem));
    EVALHASHSIZE = (long)((1048576.0 / sizeof(struct Eval)) * (hash_size / 4.0));
    Evaltt = malloc(EVALHASHSIZE * sizeof(struct Eval));
    if (!tt || !Evaltt)
    {
        printf("info string Out of memory!\n");
        fflush(stdout);
        exit(1);
    }
    clearTT();
    clearEvalTT();

    printf("info string Hash tables set to %dMB.\n", hash_size);
}

// handle options
void handle_options(char *input)
{
    char buffer[20] = "";

    input += 15;
    if (!strncmp("Hash", input, 4))
    {
        configure_hash(input);
    }
    else if (!strncmp("Clear Hash", input, 10))
    {
        // clear hash tables 
        clearTT();
        clearEvalTT();
        clearPawnTT();
        clearCounterMoveTable();
        memset(history, 0, sizeof(history)); // clear history heuristic table
    }
    else if (!strncmp("SyzygyPath", input, 10))
    {
        input += 17;
        input[strlen(input) - 1] = '\0'; // remove the new line character
        tb_init(input);
        if (TB_LARGEST > 0)
        {
            printf("info string Syzygy Path set to %s.\n", input);
        }
    }
    else if (!strncmp("SyzygyProbeDepth", input, 16))
    {
        sscanf(input, "SyzygyProbeDepth value %s\n", buffer);
        TB_DEPTH = atoi(buffer);
    }

    fflush(stdout);
}

// some GUIs might not use this command at the beginning
void handle_newgame()
{
    init_zobrist(); // generate random zobrist numbers
    clearCounterMoveTable();
    memset(history, 0, sizeof(history)); // clear history heuristic table
    outofbook = 0;

    // clear hash tables
    clearTT();
    clearEvalTT();
    clearPawnTT();
}

// parse fen notation
void parse_fen(char *position, BOARD *pos)
{
    char halfmove[20] = "";
    char fullmove[20] = "";
    int piece_count = 0;
    int x = 0, y = 0;
    clear_board(pos->board);
    position += 4;

    // loop through fen
    while (*position && x < 8)
    {
        switch (*position)
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
                printf("info string Can't parse fen!\n");
                fflush(stdout);
                exit(1);
                break;
        }
        position++;
    }
    
    // save number of pieces
    pos->piece_num = piece_count;
    
    // side to move
    if (*position == 'w')
        engine_color = -1;
    else if (*position == 'b')
        engine_color = 1;
    
    // castling rights
    position += 2;
    pos->ksb = 0;
    pos->qsb = 0;
    pos->ksw = 0;
    pos->qsw = 0;
    for (int i = 0; i < 4; i++)
    {
        switch (*position)
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
        if (*position == ' ')
            break;
    }
    
    // en passant
    position++;
    pos->ep_file = 0;
    if (*position != '-')
    {
        pos->ep_file = *position - 96;
        position++;
    }
    
    // half move counter
    position += 2;
    sscanf(position, "%s %s", halfmove, fullmove);
    pos->halfmove_counter = atoi(halfmove);
    history_index = pos->halfmove_counter;
    set_piecelists(pos);
    pos->key = getHash(pos, engine_color);
    pos->pawn_key = getPawnHash(pos->board);
    pos->pawn_push = false;
    pos->last_move.piece = NOMOVE;
    pos->last_move.to = NOMOVE;
}

void handle_position(char *input)
{
    char move[6] = "";
    MOVE smove;
    char own_piece = ' ';
    char op_piece = ' ';
    // parse the positon input
    char *position = NULL;
    char *move_str = NULL;
    const char s[2] = " ";
    memset(op_move, 0, sizeof(op_move));
    memset(history_log, -1, sizeof(history_log)); // clear history table

    // parse starting position
    if ((position = strstr(input, "startpos")))
    {
        // set the board struct to initial state
        init_board(&pos_info);
        history_log[0] = pos_info.key; // get the initial hash key
        history_index = 0;
        engine_color = -1; // white
    }
    // parse fen notation
    else if ((position = strstr(input, "fen")))
    {
        parse_fen(position, &pos_info);
    }

    // parse moves
    if ((position = strstr(input, "moves")))
    {
        move_str = strtok(position, s);
        while (move_str != NULL)
        {
            move_str = strtok(NULL, s);
            if (move_str != NULL)
            {
                memcpy(move, move_str, 6);
                move[5] = '\0';
                // convert move string to move struct
                smove = string_to_move(move);
                // make move on the board
                own_piece = pos_info.board[smove.from / 8][smove.from % 8];
                op_piece = pos_info.board[smove.to / 8][smove.to % 8];
                makeMove(&pos_info, &smove);

                // store board hash key into move history
                history_index++;
                history_log[history_index] = pos_info.key;
            }
        }
        // get opponent's move for time management
        // only when it's a capture, including en passant
        if (op_piece != ' ' || (toupper(own_piece) == 'P' && abs(smove.to % 8 - smove.from % 8) == 1))
        {
            strncpy(op_move, move, 6);
            op_move[5] = '\0';
        }
        // assign color to the engine based on the command from GUI
        if (islower(own_piece))
        {
            engine_color = -1; // white
        }
        else
        {
            engine_color = 1; // black
        }
    }
}

void handle_go(char *input)
{
    pthread_attr_t tattr;
    pthread_t thread;
    int error;
    char *pointer = NULL;
    double wt = 0, bt = 0;
    double winc = 0, binc = 0;
    int moves_left = 30;
    search_depth = -1;
    search_time = __DBL_MAX__;
    ponder_time = __DBL_MAX__;
    extra_time = true;
    analyze = false;
    node_mode = false;
    time_management = false;

    // get remaining time
    if ((pointer = strstr(input, "wtime")))
    {
        wt = (double)atoi(pointer + 6) / 1000;
    }

    if ((pointer = strstr(input, "btime")))
    {
        bt = (double)atoi(pointer + 6) / 1000;
    }

    // get increment
    if ((pointer = strstr(input, "winc")))
    {
        winc = (double)atoi(pointer + 5) / 1000;
    }

    if ((pointer = strstr(input, "binc")))
    {
        binc = (double)atoi(pointer + 5) / 1000;
    }

    // get moves left
    if ((pointer = strstr(input, "movestogo")))
    {
        // assume one more move to prevent running out of time
        moves_left = atoi(pointer + 10) + 1;
    }

    // mode
    if ((pointer = strstr(input, "ponder")))
    {
        if (engine_color == -1)
        {
            ponder_time = wt / moves_left + 0.9 * winc;
            if ((2 * ponder_time) >= wt) // prevent losing on time
                extra_time = false;
        }
        else if (engine_color == 1)
        {
            ponder_time = bt / moves_left + 0.9 * binc;
            if ((2 * ponder_time) >= bt) // prevent losing on time
                extra_time = false;
        }
    }
    else if ((pointer = strstr(input, "movetime")))
    {
        search_time = (double)atoi(pointer + 9) / 1000;
        extra_time = false;
    }
    else if ((pointer = strstr(input, "depth")))
    {
        search_depth = atoi(pointer + 6);
        // prevent depth > MAXDETPTH
        if (search_depth > MAXDEPTH)
            search_depth = MAXDEPTH;
    }
    else if ((pointer = strstr(input, "nodes")))
    {
        search_nodes = atoi(pointer + 6);
        node_mode = true;
    }
    else if ((pointer = strstr(input, "infinite")))
    {
        analyze = true;
    }
    else
    {
        // blitz / tournament time control
        // time = time left / moves left + 0.9 * increment
        // if moves left not given, assume 30 moves
        if (engine_color == -1)
        {
            search_time = wt / moves_left + 0.9 * winc;
            if ((2 * search_time) >= wt) // prevent losing on time
                extra_time = false;
        }
        else if (engine_color == 1)
        {
            search_time = bt / moves_left + 0.9 * binc;
            if ((2 * search_time) >= bt) // prevent losing on time
                extra_time = false;
        }
        if (search_time > 0 && outofbook < 5) // think longer when out of book
        {
            search_time *= 1.2;
            outofbook++;
        }
        time_management = true;
    }

    stop = false; // set stop to false
    ponderhit = false;
    // start engine thread
    pthread_attr_init(&tattr);
    pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
    error = pthread_create(&thread, &tattr, engine, NULL);

    if (error)
    {
        printf("info string Thread error occured: %d.\n", error);
        fflush(stdout);
        exit(1);
    }
}

void uci_loop()
{
    char string[4000];
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    // infinite loop for UCI GUI
    while (true)
    {
        memset(&string[0], 0, sizeof(string)); // flush the string
        fflush(stdout); // flush the stdout
        if (!fgets(string, 4000, stdin))
        {
            continue;
        }
        // skip new line
        if (string[0] == '\n')
        {
            continue;
        }
        
        // listen for command
        if (!strncmp("isready", string, 7))
        {
            printf("readyok\n");
            fflush(stdout);
        }
        else if (!strncmp("ucinewgame", string, 10))
        {
            handle_newgame();
        }
        else if (!strncmp("position", string, 8))
        {
            handle_position(string);
        }
        else if (!strncmp("go", string, 2))
        {
            handle_go(string);
        }
        else if (!strncmp("uci", string, 3))
        {
            handle_uci();
        }
        else if (!strncmp("stop", string, 4))
        {
            stop = true;
        }
        else if (!strncmp("ponderhit", string, 9))
        {
            ponderhit = true;
        }
        else if (!strncmp("setoption name", string, 14))
        {
            handle_options(string);
        }
        else if (!strncmp("quit", string, 4))
        {
            stop = true;

            #ifdef LINUX
            sleep(1);
            #else
            Sleep(300); // wait till all threads are done
            #endif

            // free tts
            if (tt != NULL)
            {
                free(tt);
                tt = NULL;
            }
            if (Evaltt != NULL)
            {
                free(Evaltt);
                Evaltt = NULL;
            }

            // free tb
            if (TB_LARGEST > 0)
            {
                tb_free();
            }
            break;
        }
    }
}

// UCI GUI
int main()
{
    // initialize variables
    tt = NULL;
    Evaltt = NULL;
    engine_color = 0;
    outofbook = 0;
    TB_DEPTH = 1;
    init_zobrist();
    init_setMask();
    init_lmr();

    // default main tt (24MB)
    HASHSIZE = (unsigned long int)((1048576.0 / sizeof(struct DataItem)) * 24);
    tt = malloc(HASHSIZE * sizeof(struct DataItem));
    clearTT();
    // default eval tt (8MB)
    EVALHASHSIZE = (unsigned long int)((1048576.0 / sizeof(struct Eval)) * 8);
    Evaltt = malloc(EVALHASHSIZE * sizeof(struct Eval));
    clearEvalTT();

    clearCounterMoveTable();
    memset(history, 0, sizeof(history));
    clearPawnTT();

    // start UCI loop
    uci_loop();

    return 0;
}
