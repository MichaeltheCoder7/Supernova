# Makefile
CC = gcc
SRC = src/*.c src/Fathom/tbprobe.c
LIBS = -lpthread -lm
WEXE = bin/Supernova_3.0
LEXE = bin/Supernova_3.0_linux
TEXE = bin/Supernova_test
RFLAGS = -std=c99 -static -flto -Ofast
DFLAGS = -std=c99 -g -Wall -Wextra -Wshadow
PSRC = $(filter-out src/Main.c, $(wildcard src/*.c tests/Perft.c))

######################## executables for release ######################
windows:
	$(CC) $(RFLAGS) $(SRC) $(LIBS) -o $(WEXE)

linux:
	$(CC) $(RFLAGS) $(SRC) $(LIBS) -o $(LEXE)

######################## executables for testing #######################
test:
	$(CC) $(RFLAGS) $(SRC) $(LIBS) -o $(TEXE)

debug:
	$(CC) $(DFLAGS) $(SRC) $(LIBS) -o $(TEXE)

perft:
	$(CC) $(RFLAGS) $(PSRC) -o $(TEXE)

############################### others #################################
run:
	$(WEXE)

run_linux:
	$(LEXE)

run_test:
	$(TEXE)