NAME=s0-wren
CC=gcc
CFLAGS=-std=c99 -c -O2 -g -Wall -Werror
LDFLAGS= -lSDL3


$(NAME): build/main.o
	$(CC) $(LDFLAGS) $< -o $@

build/main.o: src/main.c build
	$(CC) $(CFLAGS) $< -o $@ 

build:
	mkdir build

.PHONY: clean run
clean:
	rm -rf build
	rm $(NAME)

run: $(NAME)
	./$(NAME)
