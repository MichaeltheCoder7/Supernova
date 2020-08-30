#ifndef CHECKMOVE_H
#define CHECKMOVE_H

int CheckMove_wpawn(char board[8][8], int index_x, int index_y, int new_x, int new_y, char op_cp[3], char op_np[3]);

int CheckMove_bpawn(char board[8][8], int index_x, int index_y, int new_x, int new_y, char op_cp[3], char op_np[3]);

int CheckMove_wking(char board[8][8], int index_x, int index_y, int new_x, int new_y, int ksw, int qsw);

int CheckMove_bking(char board[8][8], int index_x, int index_y, int new_x, int new_y, int ksb, int qsb);

int CheckCapture_wpawn(char board[8][8], int index_x, int index_y, int new_x, int new_y, char op_cp[3], char op_np[3]);

int CheckCapture_bpawn(char board[8][8], int index_x, int index_y, int new_x, int new_y, char op_cp[3], char op_np[3]);

#endif
