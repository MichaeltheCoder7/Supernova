#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>
#include "SEE.h"
#include "Attack.h"
#include "Move.h"
#include "OrderMove.h"

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
            break;
        }

        gain[d] = piece_value(piece) - gain[d-1];
        //prunning that won't change the result
        if(((-gain[d-1] >= gain[d])? -gain[d-1]:gain[d]) < 0)
        {
            gain[d] = -gain[d-1];
            break;
        }
    }
    while(attacker_index != -1);

    while(--d)
    {
        gain[d] = -((-gain[d] >= gain[d+1])? -gain[d]:gain[d+1]);
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
        }
        
        x = attacker_index / 8;
        y = attacker_index % 8;
    }
    while(attacker_index != -1);

    while(--d)
    {
        gain[d-1] = -((-gain[d-1] >= gain[d])? -gain[d-1]:gain[d]);
    }

    return gain[0];
}