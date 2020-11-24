# Supernova

Supernova is an open-source UCI chess engine written in C. It can be used on Windows and Linux and requires a UCI compatible graphical user interface (Arena, Shredder...) to function properly. Supernova 2.1.4 is rated around 2322 Elo on [CCRL](http://ccrl.chessdom.com/ccrl/404/), and Supernova 2.2.1 is 186 Elo stronger than 2.1.4 in self-play.

Installation
------------
To download Supernova, you can download the Windows and Linux binaries in the bin directory or directly from the latest release. Alternatively, if the binaries are not compatible with your CPU, you can download all source files, go to the Supernova directory that contains src and bin directories, and compile natively using GCC, then the binary should appear in the bin directory. 

Windows:
```
gcc -std=c99 -lpthread -static -flto -Ofast ./src/*.c -o ./bin/Supernova_2.21.exe
```

Linux:
```
gcc -std=c99 -lpthread -flto -Ofast -DLINUX ./src/*.c -o ./bin/Supernova_2.21_linux
```

Note that GCC version 10 or above is preferable, and compiling might fail for GCC versions below 5. It's recommended to compile the Linux version on your own since the GCC version that was used is outdated.

[**Releases**](https://github.com/MichaeltheCoder7/Supernova/releases)  

GUI
---
To use the engine with a GUI, you need to add it to the GUI first. 
For example, if you are using Arena as GUI, instructions are given below.

1. On the taskbar, add the engine via Engines -> Install New Engine. 
2. Find and select the correct executable.
3. Select UCI as the type of engine. 
(Arena might mark it as auto-detect so you can change it to UCI later via Engines -> Manage -> Details -> select Supernova -> General -> choose UCI as type -> Apply.)
4. Start the engine.

Supernova does not have its own opening book so it's recommended to use the book provided by the GUI. Add the book by selecting Engines -> Manage -> Details -> select Supernova -> Books -> choose "Use Arena main books with this engine" -> Apply. The link to download Arena is given below.

[**Arena Download**](http://www.playwitharena.de)

Details
-------
**UCI Features** 
* Search Mode
  * Blitz
  * Tournament
  * Fixed Search Depth
  * Time per Move
  * Nodes
  * Analyze/Infinite
* FEN Support
* Pondering  
* Configurable Hash Tables from 1MB to 4096MB  
* Clear Hash Tables Option
* Search Information and Principal Variation

**Board Representation**
* 8x8 Board (mailbox)
* Piece Lists

**Move Generation**  
* All Pseudo-Legal Moves of Chess (including castling, en passant, queen promotion, and under promotions)
* Capture and Queen Promotion Generator for Quiescence Search

**Search** 
* Transposition Table
* Iterative Deepening with Aspiration Window
* Principal Variation Search
* Razoring
* Static Null Move / Reverse Futility Pruning
* Null Move Pruning
* Futility Pruning
* Probcut
* Late Move Pruning
* Late Move Reduction
* Internal Iterative Deepening
* Check Extension
* Passed Pawn Extension
* Move Ordering
* Quiescence Search
* Static Exchange Evaluation

**Evaluation** 
* Evaluation Hash Table
* Pawn Hash Table
* Piece Square Tables
* Mobility
* Trapped Pieces
* Piece-specific Evaluations
* Passed Pawn
  * Candidate
  * Connected
  * King Proximity
* King Safety
  * Pawn Shield
  * Pawn Storm
  * Semi-open Files
  * King Tropism
  * King Attack
* Game Phase

Author
------
**Minkai Yang**  

Email: mikeyang7@yahoo.com  
Free free to email me if you have any problems with Supernova.

License
-------
Supernova is covered by the MIT license. See [LICENSE](https://github.com/MichaeltheCoder7/Supernova/blob/master/LICENSE) for more details.
