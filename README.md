# Supernova

Supernova is an open source UCI chess engine written in C. It can be used on Windows and Linux and requires a UCI compatible graphical user interface(Arena, Shredder...) to function properly.

Installation
------------
To download Supernova, you can download the Windows and Linux executables in the bin directory or directly from the latest release. Alternatively, you can download all source files and Makefile and then run make outside the bin directory, then the executable should appear in the bin directory. 

Windows:
```
cd Windows
make
```

Linux:
```
cd Linux
make
```
**Releases**
https://github.com/MichaeltheCoder7/Supernova/releases  

GUI
---
To use the engine with a GUI, you need to add it to the GUI first. 
For example, if you are using Arena as GUI, instructions are given below.

1. On the task bar, add the engine via Engines -> Install New Engine. 
2. Find and select the correct executable.
3. Select UCI as the type of the engine. 
(Arena might mark it as auto-detect so you can change it to UCI later via Engines -> Manage -> Details -> select Supernova -> General -> choose UCI as type -> Apply.)
4. Start the engine.

Supernova does not have its own opening book so it's recommended to use the book provided by the GUI. Add the book by selecting Engines -> Manage -> Details -> select Supernova -> Books -> choose "Use Arena main books with this engine" -> Apply. The link to download Arena is given below.

**Arena**
http://www.playwitharena.de/

Details
-------
**UCI Features** 
* search Mode
  * blitz
  * tournament
  * fixed search depth
  * time per move
  * analyze/infinite
* pondering  
* configurable hash tables from 1MB to 2048MB  
* search information and principal variation

**Board Representation**
* 8x8 board  

**Move Generation**  
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
* evaluation hash table
* piece square tables  
* mobility  
* king safety  
* king tropism  
* piece-specific evaluations  

Author
------
**Minkai Yang**
* email: mikeyang7@yahoo.com

License
-------
Supernova is covered by the MIT license. See https://github.com/MichaeltheCoder7/Supernova/blob/master/LICENSE for more details.
