CC=gcc
CFLAGS= -lpthread
SRC=./src

genkey: keygen.o
	./keygen.o $(NPRIME) $(MPRIME) 0

keycrack: keygen.o
	./keygen.o $(E) $(C) 1
	
server: server.o
	./server.o -p $(PORT) -E $(E) -C $(C) -D $(D) -d $(DC)

client: client.o
	./client.o -s $(SERVER) -p $(PORT) -E $(E) -C $(C) -D $(D) -d $(DC)
	
client.o: $(SRC)/client.c $(SRC)/csapp.c $(SRC)/stringly.c $(SRC)/crypt.c
	gcc $^ -o $@ $(CFLAGS)

server.o: $(SRC)/server.c $(SRC)/csapp.c $(SRC)/stringly.c $(SRC)/crypt.c
	gcc $^ -o $@ $(CFLAGS)
	
keygen.o: $(SRC)/keygen.c $(SRC)/crypt.c
	gcc $^ -o $@
	
clean:
	rm keygen.o client.o server.o
