# Supernova

Supernova is an open source UCI chess engine written in C. It can be used on Windows and Linux and requires a UCI compatible graphical user interface(Arena, Winboard, Shredder...) to function properly. The strengh of Supernova is estimated to be around 2100 to 2300 Elo.

Installation
--------
To download Supernova, you can download the Windows and Linux executables in bin directory or directly from the latest release, alternatively, if you are using Windows, you can download all source files and Makefile and then run make outside the bin directory, then the executable should appear in the bin directory. To use the engine with a GUI, you need to add it to the GUI first. For example, if you are using Arena as GUI, add the engine via Engines -> Install New Engine and select the executable. Supernova does not have its own opening book so it's recommended to use the book provided by the GUI. The link to download Arena is given below.

http://www.playwitharena.de/

Details
-------
**UCI Features**
* pondering  
* analyze mode  
* configurable hash tables from 1MB to 2048MB  
* search information and principal variation  

**Board Representation**
* 8x8 board  

Move Generation:  
* all legal moves of chess including castling, en passant, queen promotion and under promotions  

**Search** 
* transposition table  
* iterative deepening with aspiration window  
* principal variation search  
* razoring  
* static null move pruning  
* null move prunning  
* futility prunning  
* late move reduction  
* check extension  
* move ordering  
* quiescence search  
* static exchange evaluation

**Evaluation** 
* piece square tables  
* mobility  
* king safety  
* king tropism  
* piece-specific evaluations  
