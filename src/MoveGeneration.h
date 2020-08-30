#ifndef MOVEGENERATION_H
#define MOVEGENERATION_H

int moveGen(char board[8][8], char all_moves[256][6], char op_cp[3], char op_np[3], int ksw, int qsw, int ksb, int qsb, int color);

int captureGen(char board[8][8], char all_moves[100][6], char op_cp[3], char op_np[3], int color);

void move_ordering(char board[8][8], char moves[256][6], int length, char pv_move[6], int ply, int color);

void cap_ordering(char board[8][8], char moves[100][6], int length, int color);

int SEE(char board[8][8], char location[3], int color);

#endif