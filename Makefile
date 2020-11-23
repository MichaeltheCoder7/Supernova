# Makefile
CC = gcc
SRC = ./src/*.c
LIBS = -lpthread
WEXE = ./bin/Supernova_2.2.1.exe
LEXE = ./bin/Supernova_2.2.1_linux
TEXE = ./bin/Supernova_test.exe
WFLAGS = -std=c99 $(LIBS) -static -flto -Ofast
LFLAGS = -std=c99 $(LIBS) -O3 -DLINUX
DFLAGS = -std=c99 $(LIBS) -static -g -Wall -Wextra -Wshadow

######################### generate the executable #####################
windows:
	$(CC) $(WFLAGS) $(SRC) -o $(WEXE)

linux:
	$(CC) $(LFLAGS) $(SRC) -o $(LEXE)

test:
	$(CC) $(WFLAGS) $(SRC) -o $(TEXE)

debug:
	$(CC) $(DFLAGS) $(SRC) -o $(WEXE)

###############################  others  ##############################
run:
	$(WEXE)

run_linux:
	$(LEXE)

run_test:
	$(TEXE)

clean:
	rm -rf $(WEXE) $(LEXE) $(TEXE)