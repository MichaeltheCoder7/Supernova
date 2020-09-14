# Makefile

# Variables
CFLAGS = gcc -ansi -std=c99 -Wall -c -O3
FLAGS = gcc -ansi -std=c99 -Wall -lpthread -O3
CDEPS = ./bin/Main.o ./bin/Board.o ./bin/CheckMove.o ./bin/Attack.o ./bin/Search.o ./bin/MoveGeneration.o ./bin/Evaluate.o

# Default target
all: ./bin/Supernova_1.3.3_linux

test: 
	./bin/Supernova_1.3.3_linux

######################### Generate object files #######################
# Target for object files
./bin/%.o: ./src/%.c
	$(CFLAGS) $^ -o $@


######################### Generate the executable #####################
# Target for Supernova
./bin/Supernova_1.3.3_linux: $(CDEPS)
	$(FLAGS) $^ -o $@

###############################  others  ##############################
# Target for clean
clean:
	rm -rf ./bin/*.o $(TXT) ./bin/Supernova_1.3.3_linux
