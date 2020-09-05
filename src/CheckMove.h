#ifndef CHECKMOVE_H
#define CHECKMOVE_H

int CheckMove_wpawn(char board[8][8], int index_y, int new_x, int new_y, char op_cp[3], char op_np[3]);

int CheckMove_bpawn(char board[8][8], int index_y, int new_x, int new_y, char op_cp[3], char op_np[3]);

int CheckMove_wkingside(char board[8][8], int ksw);

int CheckMove_wqueenside(char board[8][8], int qsw);

int CheckMove_bkingside(char board[8][8], int ksb);

int CheckMove_bqueenside(char board[8][8], int qsb);

int CheckCapture_wpawn(char board[8][8], int new_x, int new_y, char op_cp[3], char op_np[3]);

int CheckCapture_bpawn(char board[8][8], int new_x, int new_y, char op_cp[3], char op_np[3]);

#endif
