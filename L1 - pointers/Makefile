SRCS = $(wildcard *.c)
PROGS = $(patsubst %.c,%.o,$(SRCS))
OBJS = $(wildcard *.o)
CC=gcc

make: all run

all: $(PROGS)

%.o: %.c
	$(CC) -o $@ $<

run: $(PROGS)
	for file in $(PROGS); do echo "\nExecuting $$file\n" && $(CURDIR)/$$file; done;

clean:
	rm -rf ./*.o

problem_1: number_1.o
	@echo "Executing problem 1"
	$(CURDIR)/number_1.o

problem_2: number_2.o
	@echo "Executing problem 2"
	$(CURDIR)/number_2.o

problem_3: number_3.o
	@echo "Executing problem 3"
	$(CURDIR)/number_3.o

problem_4: number_4.o
	@echo "Executing problem 4"
	$(CURDIR)/number_4.o

problem_5: number_5.o
	@echo "Executing problem 5"
	$(CURDIR)/number_5.o