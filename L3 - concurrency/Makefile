CC=gcc
CFLAGS= -lpthread

runall: build run_no_locks run_locks run_locks_loop
	
run_no_locks: run.sh lab3_no_locks.o
	@echo "Running program 10 times with no locks...\n"
	sh run.sh ./lab3_no_locks.o
	
run_locks: run.sh lab3_locks.o
	@echo "Running program 10 times with locks inside of loop...\n"
	sh run.sh ./lab3_locks.o
	
run_locks_loop: run.sh lab3_locks_loop.o
	@echo "Running program 10 times with locks outside of loop...\n"
	sh run.sh ./lab3_locks_loop.o	
	
build: lab3_no_locks.o lab3_locks.o lab3_locks_loop.o
	
lab3_locks_loop.o: lab3_locks_loop.c
	$(CC) $^ -o $@ $(CFLAGS)
	
lab3_no_locks.o: lab3_no_locks.c
	$(CC) $^ -o $@ $(CFLAGS)

lab3_locks.o: lab3_locks.c
	$(CC) $^ -o $@ $(CFLAGS)
	
clean:
	rm *.o