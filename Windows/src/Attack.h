#ifndef ATTACK_H
#define ATTACK_H

int ifCheck(char board[8][8], int color);

int isThreatened(char board[8][8], char location[3], int color);

int get_smallest_attacker(char board[8][8], char location[3], int color);

#endif
