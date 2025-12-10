CC=gcc
CFLAGS=-g -Wall
LIBS=-lncurses -pthread

SRC=main.c gui/htop.c task/do_nothing.c interrupt_handler/fork_exec.c interrupt_handler/tick.c
OBJ=$(SRC:.c=.o)

all: main

main: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o main $(LIBS)

clean:
	rm -f *.o main
