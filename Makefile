NAME=s0-wren
CC=gcc
CFLAGS=-std=c99 -c -g -Wall -Werror
LDFLAGS=-lSDL3 -lm
OBJS=build/main.o build/maths.o build/sowren.o build/const.o


$(NAME): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

build/main.o: src/main.c src/base.h src/const.h build
	$(CC) $(CFLAGS) $< -o $@ 

build/maths.o: src/maths.c src/maths.h src/base.h src/const.h build
	$(CC) $(CFLAGS) $< -o $@ 

build/sowren.o: src/sowren.c src/sowren.h src/base.h src/const.h build
	$(CC) $(CFLAGS) $< -o $@ 

build/const.o: src/const.c src/base.h build
	$(CC) $(CFLAGS) $< -o $@ 

build:
	mkdir build

.PHONY: clean run
clean:
	rm -rf build
	rm $(NAME)

run: $(NAME)
	./$(NAME)
