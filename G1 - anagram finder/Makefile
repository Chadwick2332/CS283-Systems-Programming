CC=gcc

run: build
	./main.o anagram begin

anagram: build
	./main.o anagram $(WORD)

scrabble: build
	./main.o anagram $(WORD) $(LETTER) $(PLACE)

permutations: build
	./main.o permutations $(WORD)

position-permutations: build
	./main.o permutations $(WORD) $(LETTER) $(PLACE)

build: main.o

main.o: main.c
	 $(CC) $^ -o $@

clean:
	rm main.o