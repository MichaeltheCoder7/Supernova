# Makefile

# Variables
CFLAGS = gcc -ansi -std=c99 -Wall -c -O3
FLAGS = gcc -ansi -std=c99 -Wall -O3
CDEPS = ./bin/Main.o ./bin/Board.o ./bin/CheckMove.o ./bin/Checkmate.o ./bin/Search.o ./bin/MoveGeneration.o ./bin/Evaluate.o
# Default target
all: ./bin/Supernova

test: 
	./bin/Supernova

######################### Generate object files #######################
# Target for object files
./bin/%.o: ./src/%.c
	$(CFLAGS) $^ -o $@


######################### Generate the executable #####################
# Target for Supernova
./bin/Supernova: $(CDEPS)
	$(FLAGS) $^ -o $@

###############################  others  ##############################
# Target for clean
clean:
	rm -rf ./bin/*.o ./bin/Supernova
