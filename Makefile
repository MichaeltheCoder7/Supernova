# Makefile
CC = gcc
SRC = ./src/*.c
LIBS = -lpthread
WEXE = ./bin/Supernova_2.3.exe
LEXE = ./bin/Supernova_2.3_linux
WFLAGS = -std=c99 $(LIBS) -static -flto -Ofast
LFLAGS = -std=c99 $(LIBS) -O3 -DLINUX
DFLAGS = -std=c99 $(LIBS) -static -g -Wall -Wextra -Wshadow

######################### generate the executable #####################
windows:
	$(CC) $(WFLAGS) $(SRC) -o $(WEXE)

linux:
	$(CC) $(LFLAGS) $(SRC) -o $(LEXE)

debug:
	$(CC) $(DFLAGS) $(SRC) -o $(WEXE)

###############################  others  ##############################
test:
	$(WEXE)

test_linux:
	$(LEXE)

clean:
	rm -rf $(WEXE) $(LEXE)