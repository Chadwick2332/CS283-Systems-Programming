CC = gcc

all: run clean

run: build
	export DIMENSION=$(DIMENSION)
	export GAMES=$(GAMES)
	./main.o
	
test: build
	export DIMENSION=8
	export GAMES=3
	./main.o

build:
	$(CC) -o main.o main.c game-lib.c

clean:
	rm *.o
