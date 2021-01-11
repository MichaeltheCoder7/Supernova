#ifndef MOBILITY_H
#define MOBILITY_H

extern int wattack_count;
extern int battack_count;
extern int wattack_weight;
extern int battack_weight;

int wknight_mobility(unsigned char board[8][8], int index_x, int index_y, int bkingx, int bkingy);

int bknight_mobility(unsigned char board[8][8], int index_x, int index_y, int wkingx, int wkingy);

int wbishop_mobility(unsigned char board[8][8], int index_x, int index_y, int bkingx, int bkingy);

int bbishop_mobility(unsigned char board[8][8], int index_x, int index_y, int wkingx, int wkingy);

int wrook_mobility(unsigned char board[8][8], int index_x, int index_y, int bkingx, int bkingy);

int brook_mobility(unsigned char board[8][8], int index_x, int index_y, int wkingx, int wkingy);

int wqueen_mobility(unsigned char board[8][8], int index_x, int index_y, int bkingx, int bkingy);

int bqueen_mobility(unsigned char board[8][8], int index_x, int index_y, int wkingx, int wkingy);

#endif