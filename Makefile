# Makefile

# Variables
CC = gcc
CFLAGS = -Wall -Wextra -g -lm
BIN_DIR = bin
SRC_DIR = src

# Default target
all: $(BIN_DIR)/program

# Compile source files into object files
$(BIN_DIR)/program: main.o
	$(CC) $(CFLAGS) -o $(BIN_DIR)/main main.o

main.o: $(SRC_DIR)/main.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/main.c

# Clean build files
clean:
	rm -f *.o $(BIN_DIR)/main
