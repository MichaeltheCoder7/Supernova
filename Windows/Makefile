# Makefile
CC = gcc
SRC = ./src/*.c
LIBS = -lpthread
EXE = ./bin/Supernova_2.2.exe
RFLAGS = -std=c99 $(LIBS) -static -flto -Ofast
DFLAGS = -std=c99 $(LIBS) -static -g -Wall -Wextra -Wshadow

######################### generate the executable #####################
release:
	$(CC) $(RFLAGS) $(SRC) -o $(EXE)

debug:
	$(CC) $(DFLAGS) $(SRC) -o $(EXE)

###############################  others  ##############################
test:
	$(EXE)

clean:
	rm -rf $(EXE)