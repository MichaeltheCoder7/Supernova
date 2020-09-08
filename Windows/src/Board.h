#ifndef BOARD__H
#define BOARD__H

void displayboard(char board[8][8]);

int position_to_x(char position[3]);

int position_to_y(char position[3]);

char position_to_piece(char board[8][8], char current_position[3]);

int make_move(char cur_p[3], char new_p[3], char piece, char ep, char board[8][8]);

int makeMove(char cur_p[3], char new_p[3], char promotion, char piece, char ep, char board[8][8]);

void resetboard(char board[8][8]);

#endif
