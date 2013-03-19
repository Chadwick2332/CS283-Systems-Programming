CC=gcc
LIBS = -lpthread
LIBDIR = $(CURDIR)/lib
SRCDIR = $(CURDIR)/src

all: linked_list philosophers barrier

run: run_linked_list run_philosophers run_barrier

run_linked_list: linked_list
	./linked_list.o

run_philosophers: philosophers
	./philosophers.o

run_barrier: barrier
	./barrier.o

linked_list: $(SRCDIR)/linked_list.c
	$(CC) $(SRCDIR)/linked_list.c $(LIBS) -o linked_list.o

philosophers: $(SRCDIR)/philosophers.c
	$(CC) $^ $(LIBS) -o philosophers.o

barrier: $(SRCDIR)/barrier_example.c $(SRCDIR)/barrier.c
	$(CC) -c $(SRCDIR)/barrier.c -o barrier.o
	ar rvs $(LIBDIR)/libbarrier.a barrier.o
	rm barrier.o
	$(CC) $(SRCDIR)/barrier_example.c $(LIBS) -L $(LIBDIR) -l barrier -o barrier.o

clean: 
	rm -f *.o