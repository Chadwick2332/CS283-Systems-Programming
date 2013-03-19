CC=gcc
CFLAGS= -lpthread

run_port: build
	./web_service.o $(PORT)

run: build
	./web_service.o

build: web_service.o

web_service.o: web_service.c csapp.c stringly.c
	$(CC) $^ -o $@ $(CFLAGS)
	
clean:
	rm *.o