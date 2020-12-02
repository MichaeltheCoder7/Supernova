#ifndef MOBILITY_H 
#define MOBILITY_H

extern int wattack_count;
extern int battack_count;
extern int wattack_weight;
extern int battack_weight;

int wknight_mobility(char board[8][8], int index_x, int index_y, int bkingx, int bkingy);

int bknight_mobility(char board[8][8], int index_x, int index_y, int wkingx, int wkingy);

int wbishop_mobility(char board[8][8], int index_x, int index_y, int bkingx, int bkingy);

int bbishop_mobility(char board[8][8], int index_x, int index_y, int wkingx, int wkingy);

int wrook_mobility(char board[8][8], int index_x, int index_y, int bkingx, int bkingy);

int brook_mobility(char board[8][8], int index_x, int index_y, int wkingx, int wkingy);

int wqueen_mobility(char board[8][8], int index_x, int index_y, int bkingx, int bkingy);

int bqueen_mobility(char board[8][8], int index_x, int index_y, int wkingx, int wkingy);

#endif