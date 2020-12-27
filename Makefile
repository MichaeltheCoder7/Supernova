# Makefile
CC = gcc
SRC = src/*.c src/Fathom/tbprobe.c
LIBS = -lpthread -lm
WEXE = bin/Supernova_dev.exe
LEXE = bin/Supernova_dev_linux
TEXE = bin/Supernova_test.exe
WFLAGS = -std=c99 $(LIBS) -static -flto -Ofast
LFLAGS = -std=c99 $(LIBS) -O3 -DLINUX
DFLAGS = -std=c99 $(LIBS) -static -g -Wall -Wextra -Wshadow
PSRC = $(filter-out src/Main.c, $(wildcard src/*.c tests/Perft.c))

######################## executables for release ######################
windows:
	$(CC) $(WFLAGS) $(SRC) -o $(WEXE)

linux:
	$(CC) $(LFLAGS) $(SRC) -o $(LEXE)

######################## executables for testing #######################
test:
	$(CC) $(WFLAGS) $(SRC) -o $(TEXE)

debug:
	$(CC) $(DFLAGS) $(SRC) -o $(TEXE)

perft:
	$(CC) $(WFLAGS) $(PSRC) -o $(TEXE)

############################### others #################################
run:
	$(WEXE)

run_linux:
	$(LEXE)

run_test:
	$(TEXE)

clean:
	rm -rf $(WEXE) $(LEXE) $(TEXE)