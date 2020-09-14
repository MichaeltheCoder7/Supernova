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
* Search Mode
  * Blitz
  * Tournament
  * Fixed Search Depth
  * Time per Move
  * Analyze/Infinite
* Pondering  
* Configurable Hash Tables from 1MB to 2048MB  
* Search Information and Principal Variation

**Board Representation**
* 8x8 Board  

**Move Generation**  
* All Legal Moves of Chess (including castling, en passant, queen promotion and under promotions) 

**Search** 
* Transposition Table  
* Iterative Deepening with Aspiration Window  
* Principal Variation Search  
* Razoring  
* Static Null Move Pruning  
* Null Move Prunning  
* Futility Prunning  
* Late Move Reduction  
* Check Extension  
* Move Ordering  
* Quiescence Search  
* Static Exchange Evaluation

**Evaluation** 
* Evaluation Hash Table
* Piece Square Tables  
* Mobility  
* King Safety  
* King Tropism  
* Piece-specific Evaluations  

Author
------
**Minkai Yang**
* Email: mikeyang7@yahoo.com

License
-------
Supernova is covered by the MIT license. See https://github.com/MichaeltheCoder7/Supernova/blob/master/LICENSE for more details.
