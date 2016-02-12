# Compilation Parameters
CC=gcc
CFLAGS= -Iincludes -Wextra -Wall
SOURCES= main.c comm.c proto.c
BIN_NAME= execmon
all:
	$(CC) $(SOURCES) $(CFLAGS) -o $(BIN_NAME)
	cp $(BIN_NAME) ../$(BIN_NAME)
clean:
	rm -f ../$(BIN_NAME)
