#ifndef PSQT_H
#define PSQT_H

static const int passedpawn_black[8] = { 0, 15, 20, 32, 56, 92, 140, 0 };
static const int passedpawn_white[8] = { 0, 140, 92, 56, 32, 20, 15, 0 };
static const int knight_val[9] = { -20, -16, -12, -8, -4,  0,  4,  8, 10 };
static const int rook_val[9] = { 15, 12,  9,  6,  3,  0, -3, -6, -9 };

static const int safety_table[100] = {

                    0,   0,   1,   2,   3,   4,   6,   8,  10,  12,
                    15,  18,  21,  24,  28,  32,  36,  40,  45,  50,
                    55,  60,  66,  72,  78,  84,  91,  98, 105, 112,
                    120, 128, 136, 144, 153, 162, 171, 180, 190, 200,
                    210, 220, 231, 242, 253, 264, 276, 288, 300, 312,
                    324, 336, 348, 360, 372, 384, 395, 400, 400, 400,
                    400, 400, 400, 400, 400, 400, 400, 400, 400, 400,
                    400, 400, 400, 400, 400, 400, 400, 400, 400, 400,
                    400, 400, 400, 400, 400, 400, 400, 400, 400, 400,
                    400, 400, 400, 400, 400, 400, 400, 400, 400, 400

};

// piece-square tables:
// add bonus points based on position
static const int white_pawn_midgame[8][8] = {

                    {  0,   0,	 0,   0,   0,   0,   0,	  0 },
                    { 20,  20,	20,  30,  30,  20,  20,	 20 },
                    { 10,  10,	10,  20,  20,  10,  10,	 10 },
                    {  5,   5,	 5,  10,  10,	5,   5,	  5 },
                    {  0,   0,	10,  20,  20,  10,   0,	  0 },
                    {  5,   0,	 0,   5,   5,	0,   0,	  5 },
                    { 10,  10,	 5, -15, -15,   5,  10,	 10 },
                    {  0,   0,	 0,   0,   0,	0,   0,	  0 },

};

static const int white_pawn_endgame[8][8] = {

                    {  0,   0,	 0,   0,   0,   0,   0,	  0 },
                    { 20,  20,	20,  20,  20,  20,  20,	 20 },
                    { 10,  10,	10,  10,  10,  10,  10,	 10 },
                    {  5,   5,	 5,   5,   5,	5,   5,	  5 },
                    {  0,   0,	 0,   0,   0,   0,   0,	  0 },
                    { -5,  -5,  -5,  -5,  -5,  -5,  -5,	 -5 },
                    {-10, -10, -10, -10, -10, -10, -10,	-10 },
                    {  0,   0,	 0,   0,   0,	0,   0,	  0 },

};

static const int white_knight[8][8] = {

                    { -10, -10, -10, -10, -10, -10, -10, -10 },
                    { -10,   0,   0,   0,   0,   0,   0, -10 },
                    { -10,   0,   5,   5,   5,   5,   0, -10 },
                    { -10,   0,   5,  10,  10,   5,   0, -10 },
                    { -10,   0,   5,  10,  10,   5,   0, -10 },
                    { -10,   0,   5,   5,   5,   5,   0, -10 },
                    { -10,   0,   0,   0,   0,   0,   0, -10 },
                    { -10, -30, -10, -10, -10, -10, -30, -10 },

};


static const int white_bishop[8][8] = {

                    { -10, -10, -10, -10, -10, -10, -10, -10 },
                    { -10,   0,   0,   0,   0,   0,   0, -10 },
                    { -10,   0,   5,   5,   5,   5,   0, -10 },
                    { -10,   0,   5,  10,  10,   5,   0, -10 },
                    { -10,   0,   5,  10,  10,   5,   0, -10 },
                    { -10,   0,   5,   5,   5,   5,   0, -10 },
                    { -10,   0,   0,   0,   0,   0,   0, -10 },
                    { -10, -10, -20, -10, -10, -20, -10, -10 },

};

static const int white_rook[8][8] = {

                    {  5,   5,   5,   5,   5,   5,   5,   5 },
                    { 20,  20,  20,  20,  20,  20,  20,  20 },
                    { -5,   0,   0,   0,   0,   0,   0,  -5 },
                    { -5,   0,   0,   0,   0,   0,   0,  -5 },
                    { -5,   0,   0,   0,   0,   0,   0,  -5 },
                    { -5,   0,   0,   0,   0,   0,   0,  -5 },
                    { -5,   0,   0,   0,   0,   0,   0,  -5 },
                    {  0,   0,   0,   2,   2,   0,   0,   0 },

};

static const int white_queen[8][8] = {

                    { -20, -10, -10,  -5,  -5, -10, -10, -20 },
                    { -10,   0,   0,   0,   0,   0,   0, -10 },
                    { -10,   0,   5,   5,   5,   5,   0, -10 },
                    {  -5,   0,   5,   5,   5,   5,   0,  -5 },
                    {   0,   0,   5,   5,   5,   5,   0,  -5 },
                    { -10,   5,   5,   5,   5,   5,   0, -10 },
                    { -10,   0,   5,   0,   0,   0,   0, -10 },
                    { -20, -10, -10,  -5,  -5, -10, -10, -20 },

};

static const int white_king_midgame[8][8] = {

                    { -30, -40, -40, -50, -50, -40, -40, -30 },
                    { -30, -40, -40, -50, -50, -40, -40, -30 },
                    { -30, -40, -40, -50, -50, -40, -40, -30 },
                    { -30, -40, -40, -50, -50, -40, -40, -30 },
                    { -20, -30, -30, -40, -40, -30, -30, -20 },
                    { -10, -20, -20, -20, -20, -20, -20, -10 },
                    {  20,  20,   0,   0,   0,   0,  20,  20 },
                    {  30,  40,  20,   0,   0,  20,  40,  30 },

};

static const int white_king_endgame[8][8] = {

                    { -72, -48, -36, -24, -24, -36, -48, -72 },
                    { -48, -24, -12,   0,   0, -12, -24, -48 },
                    { -36, -12,   0,  12,  12,   0, -12, -36 },
                    { -24,   0,  12,  24,  24,  12,   0, -24 },
                    { -24,   0,  12,  24,  24,  12,   0, -24 },
                    { -36, -12,   0,  12,  12,   0, -12, -36 },
                    { -48, -24, -12,   0,   0, -12, -24, -48 },
                    { -72, -48, -36, -24, -24, -36, -48, -72 },

};

// for black:
static const int black_pawn_midgame[8][8] = {

                    {  0,   0,	 0,   0,   0,	0,   0,	  0 },
                    { 10,  10,	 5, -15, -15,   5,  10,	 10 },
                    {  5,   0,	 0,   5,   5,	0,   0,	  5 },
                    {  0,   0,	10,  20,  20,  10,   0,	  0 },
                    {  5,   5,	 5,  10,  10,   5,   5,	  5 },
                    { 10,  10,	10,  20,  20,  10,  10,	 10 },
                    { 20,  20,	20,  30,  30,  20,  20,	 20 },
                    {  0,   0,   0,   0,   0,   0,   0,	  0 },

};

static const int black_pawn_endgame[8][8] = {

                    {  0,   0,	 0,   0,   0,   0,   0,	  0 },
                    {-10, -10, -10, -10, -10, -10, -10,	-10 },
                    { -5,  -5,  -5,  -5,  -5,  -5,  -5,	 -5 },
                    {  0,   0,	 0,   0,   0,   0,   0,	  0 },
                    {  5,   5,	 5,   5,   5,	5,   5,	  5 },
                    { 10,  10,	10,  10,  10,  10,  10,	 10 },
                    { 20,  20,	20,  20,  20,  20,  20,	 20 },
                    {  0,   0,	 0,   0,   0,	0,   0,	  0 },

};

static const int black_knight[8][8] = {

                    { -10, -30, -10, -10, -10, -10, -30, -10 },
                    { -10,   0,   0,   0,   0,   0,   0, -10 },
                    { -10,   0,   5,   5,   5,   5,   0, -10 },
                    { -10,   0,   5,  10,  10,   5,   0, -10 },
                    { -10,   0,   5,  10,  10,   5,   0, -10 },
                    { -10,   0,   5,   5,   5,   5,   0, -10 },
                    { -10,   0,   0,   0,   0,   0,   0, -10 },
                    { -10, -10, -10, -10, -10, -10, -10, -10 },

};

static const int black_bishop[8][8] = {

                    { -10, -10, -20, -10, -10, -20, -10, -10 },
                    { -10,   0,   0,   0,   0,   0,   0, -10 },
                    { -10,   0,   5,   5,   5,   5,   0, -10 },
                    { -10,   0,   5,  10,  10,   5,   0, -10 },
                    { -10,   0,   5,  10,  10,   5,   0, -10 },
                    { -10,   0,   5,   5,   5,   5,   0, -10 },
                    { -10,   0,   0,   0,   0,   0,   0, -10 },
                    { -10, -10, -10, -10, -10, -10, -10, -10 },

};

static const int black_rook[8][8] = {

                    {  0,   0,   0,   2,   2,   0,   0,   0 },
                    { -5,   0,   0,   0,   0,   0,   0,  -5 },
                    { -5,   0,   0,   0,   0,   0,   0,  -5 },
                    { -5,   0,   0,   0,   0,   0,   0,  -5 },
                    { -5,   0,   0,   0,   0,   0,   0,  -5 },
                    { -5,   0,   0,   0,   0,   0,   0,  -5 },
                    { 20,  20,  20,  20,  20,  20,  20,  20 },
                    {  5,   5,   5,   5,   5,   5,   5,   5 },

};

static const int black_queen[8][8] = {

                    { -20, -10, -10,  -5,  -5, -10, -10, -20 },
                    { -10,   0,   5,   0,   0,   0,   0, -10 },
                    { -10,   5,   5,   5,   5,   5,   0, -10 },
                    {   0,   0,   5,   5,   5,   5,   0,  -5 },
                    {  -5,   0,   5,   5,   5,   5,   0,  -5 },
                    { -10,   0,   5,   5,   5,   5,   0, -10 },
                    { -10,   0,   0,   0,   0,   0,   0, -10 },
                    { -20, -10, -10,  -5,  -5, -10, -10, -20 },

};

static const int black_king_midgame[8][8] = {

                    {  30,  40,  20,   0,   0,  20,  40,  30 },
                    {  20,  20,   0,   0,   0,   0,  20,  20 },
                    { -10, -20, -20, -20, -20, -20, -20, -10 },
                    { -20, -30, -30, -40, -40, -30, -30, -20 },
                    { -30, -40, -40, -50, -50, -40, -40, -30 },
                    { -30, -40, -40, -50, -50, -40, -40, -30 },
                    { -30, -40, -40, -50, -50, -40, -40, -30 },
                    { -30, -40, -40, -50, -50, -40, -40, -30 },

};

static const int black_king_endgame[8][8] = {

                    { -72, -48, -36, -24, -24, -36, -48, -72 },
                    { -48, -24, -12,   0,   0, -12, -24, -48 },
                    { -36, -12,   0,  12,  12,   0, -12, -36 },
                    { -24,   0,  12,  24,  24,  12,   0, -24 },
                    { -24,   0,  12,  24,  24,  12,   0, -24 },
                    { -36, -12,   0,  12,  12,   0, -12, -36 },
                    { -48, -24, -12,   0,   0, -12, -24, -48 },
                    { -72, -48, -36, -24, -24, -36, -48, -72 },

};

#endif