#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Board.h"
#include "CheckMove.h"
#include "Attack.h"

/* 1 for legal move, 0 for illegal move*/
//captures for white pawn
int CheckCapture_wpawn(BOARD *pos, int new_x, int new_y)
{
    //capturing
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

    return 0;
}

//captures for black pawn
int CheckCapture_bpawn(BOARD *pos, int new_x, int new_y)
{
    //capturing
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
                    
    return 0;
}

//castling for white king
int CheckMove_wkingside(BOARD *pos)
{
    //king side
    //check if king and rook have been moved
    if(pos->ksw)
    {
        //check if positions between king and rook are empty
        if(pos->board[7][5] == ' ' && pos->board[7][6] == ' ')
        {
            //check if king is in check and the two other spots are threatened
            if(!ifCheck(pos, -1) && !isThreatened(pos->board, 7, 5, -1) && !isThreatened(pos->board, 7, 6, -1))
            {
                return 1;
            }
        }   
    }

    return 0; 
}

//castling for white king
int CheckMove_wqueenside(BOARD *pos)
{
    //queen side
    //check if king and rook have been moved
    if(pos->qsw)
    {
        //check if positions between king and rook are empty
        if(pos->board[7][1] == ' ' && pos->board[7][2] == ' ' && pos->board[7][3] == ' ')
        {
            //check if king is in check and the two other spots are threatened
            if(!ifCheck(pos, -1) && !isThreatened(pos->board, 7, 2, -1) && !isThreatened(pos->board, 7, 3, -1))
            {
                return 1;      
            }
        } 
    }
 
    return 0; 
}

//castling for black king
int CheckMove_bkingside(BOARD *pos)
{
    //king side
    //check if king and rook have been moved
    if(pos->ksb)
    {
        //check if positions between king and rook are empty
        if(pos->board[0][5] == ' ' && pos->board[0][6] == ' ')
        {
            //check if king is in check and the two other spots are threatened
            if(!ifCheck(pos, 1) && !isThreatened(pos->board, 0, 5, 1) && !isThreatened(pos->board, 0, 6, 1))
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
    //queen side
    //check if king and rook have been moved
    if(pos->qsb)
    {
        //check if positions between king and rook are empty
        if(pos->board[0][1] == ' ' && pos->board[0][2] == ' ' && pos->board[0][3] == ' ')
        {
            //check if king is in check and the two other spots are threatened
            if(!ifCheck(pos, 1) && !isThreatened(pos->board, 0, 2, 1) && !isThreatened(pos->board, 0, 3, 1))
            {
                return 1;
            }
        } 
    }

    return 0; 
}