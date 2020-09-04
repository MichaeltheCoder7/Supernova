# Supernova

Supernova is an open source UCI chess engine written in C. It can be used on Windows and Linux and requires a UCI compatible graphical user interface(Arena, Winboard, Shredder...) to function properly. The strengh of the engine is estimated to be around 2100 to 2300 elo.

UCI Features:
pondering
analyze mode
configurable hash tables from 1MB to 2048MB
search information and principal variation

Board Representation:
8x8 board

Move Generation:
all legal moves of chess including castling, en passant, queen promotion and under promotions

Search:
transposition table
iterative deepening with aspiration window
principal variation search
razoring
static null move pruning
null move prunning
futility prunning
late move reduction
check extension
move ordering
quiescence search

Evaluation:
piece square table
mobility
king safety
king tropism
piece-specific evaluations

To download the engine, you can download the executables in bin directory or Supernova.zip directly from 1.1.0 release, or alternatively, you can download all source files and Makefile and then run make on windows. To use the engine with a GUI, you need to add it to the GUI first. For example, if you are using Arena as GUI, add the engine via Engines -> Install New Engine and select the executable. The link to download Arena is given below.

http://www.playwitharena.de/
