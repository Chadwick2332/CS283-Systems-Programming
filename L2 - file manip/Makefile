CC=gcc
CFLAGS=-lpthread -g
SRC=./src

test: sync.o test_area.zip
	sh bin/quick_run.sh

sync: sync.o
	./sync.o $(DIR1) $(DIR)

sync.o: $(SRC)/sync.c $(SRC)/csapp.c
	$(CC) $^ -o $@ $(CFLAGS)

clean:
	rm -rf *.o
	rm -rf a
	rm -rf bin