#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CheckMove.h"
#include "Board.h"
#include "Checkmate.h"


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
