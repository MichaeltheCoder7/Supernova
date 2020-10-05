# Makefile

# Variables
CFLAGS = gcc -ansi -std=c99 -c -O3
FLAGS = gcc -ansi -std=c99 -lpthread -O3
CDEPS = ./bin/Main.o ./bin/Board.o ./bin/CheckMove.o ./bin/MoveGen.o ./bin/OrderMove.o ./bin/Attack.o ./bin/Evaluate.o ./bin/Transposition.o ./bin/Search.o

# Default target
all: ./bin/Supernova_2.0_linux

test: 
	./bin/Supernova_2.0_linux

######################### Generate object files #######################
# Target for object files
./bin/%.o: ./src/%.c
	$(CFLAGS) $^ -o $@

######################### Generate the executable #####################
# Target for Supernova
./bin/Supernova_2.0_linux: $(CDEPS)
	$(FLAGS) $^ -o $@

###############################  others  ##############################
# Target for clean
clean:
	rm -rf ./bin/*.o ./bin/Supernova_2.0_linux
