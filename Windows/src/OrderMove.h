#ifndef ORDERMOVE_H
#define ORDERMOVE_H

int wCapMove_score(char move[6], char piece, char op_piece, char board[8][8], int x, int y, char np[3]);

int bCapMove_score(char move[6], char piece, char op_piece, char board[8][8], int x, int y, char np[3]);

int promotion_score();

int underpromotion_score();

int castling_score();

int quietMove_score(char move[6], int x1, int y1, int x2, int y2, int ply, int color);

int wCapMove_score_root(char move[6], char best_move[6], char hash_move[6], char piece, char op_piece, char board[8][8], int x, int y, char np[3]);

int bCapMove_score_root(char move[6], char best_move[6], char hash_move[6], char piece, char op_piece, char board[8][8], int x, int y, char np[3]);

int promotion_score_root(char move[6], char best_move[6], char hash_move[6]);

int underpromotion_score_root(char move[6], char best_move[6], char hash_move[6]);

int castling_score_root(char move[6], char best_move[6], char hash_move[6]);

int quietMove_score_root(char move[6], char best_move[6], char hash_move[6], int x1, int y1, int x2, int y2, int color);

int wCapQsearch_score(char piece, char op_piece, int new_x);

int bCapQsearch_score(char piece, char op_piece, int new_x);

int piece_value(char piece);

void movesort(char moves[256][6], int sort[256], int length, int current);

int orderHashMove(char moves[256][6], int sort[256], int length, char hash_move[6]);

#endif