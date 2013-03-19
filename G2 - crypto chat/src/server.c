#include <stdlib.h>
#include <stdio.h>
#include "csapp.h"

// Maximum message size is 2048 characters
const int MAX_MESSAGE_SIZE = 2048;
// Server can support 100 concurrent connections by default
const int MAX_CONNECTIONS = 100;

/** Client struct
 *   struct key* public  : Client's public RSA key
 *   int fd              : Client's file descriptor
 *   int buddy           : File descriptor of current chat buddy
 *   char username[16]   : 16 character username string
 **/
struct client {
  struct key* public;
  int fd;
  int buddy;
  char username[16];
};

// RSA key struct
struct key {
	long a;
  long b;
};

void* client_process(void* arg);
void chat(struct client * sender, struct client * receiver);
//void chat(int sendfd, int receivefd);
struct client* find_client_by_fd(int fd);

/** GLOBALS **/
int connections = 0;
struct client* clients;

int main (int argc, char ** argv)
{
	int listenfd, connfd;
  socklen_t clientlen;
	struct sockaddr_in clientaddr;
	pthread_t threads;
  clients = (struct client*) malloc(sizeof(struct client) * MAX_CONNECTIONS);
	char* haddrp;
	int port = 4567;
  int k, e = 0, c = 0, d = 0, dc = 0;
  
  while((k = getopt(argc, argv, "p::E::C::D::d::")) != -1) {
		switch(k) {
			case 'p':
				if(optarg != 0) {
					port = atoi(optarg);
				}
				break;
			case 'E':
				if(optarg != 0) {
					e = atoi(optarg);
				}
				break;
			case 'C':
				if(optarg != 0) {
					c = atoi(optarg);
				}
				break;
			case 'D':
				if(optarg != 0) {
					d = atoi(optarg);
				}
				break;
			case 'd':
				if(optarg != 0) {
					dc = atoi(optarg);
				}
				break;
		}
	}
  
	// Open up server on port
	listenfd = open_listenfd(port);
	printf("Server running on port %d\n", port);
  
  // Start main server loop
	while(1) {
		clientlen = sizeof(clientaddr);
    // Wait for incoming connections
		connfd = Accept(listenfd, (SA*) &clientaddr, &clientlen);
    // Print newly connected client's IP to log
    haddrp = inet_ntoa(clientaddr.sin_addr);
		printf("client: %s\n", haddrp);
		//hp = Gethostbyaddr(inet_ntoa(clientaddr.sin_addr), sizeof(clientaddr.sin_addr.s_addr), AF_INET);
		if(connfd == -1) {
			fprintf(stderr, "Unable to accept new connection\n");
		} else {
      int i, clientIndex;
      for(i = 0; i < MAX_CONNECTIONS; i++) {
        // open connection
        if(clients[i].fd == 0) {
          clients[i].fd = connfd;
          clients[i].buddy = 0;
          clientIndex = i;
          break;
        }
      }
      
      // find first open buddy
      for(i = 0; i < MAX_CONNECTIONS; i++) {
        if(clients[i].fd != 0 && clients[i].buddy == 0 && i != clientIndex) {
          clients[clientIndex].buddy = clients[i].fd;
          clients[i].buddy = clients[clientIndex].fd;
        }
      }
      
      // Buddy connecting logic...CHANGE THIS CHRISTIAN
      clients[connections].fd = connfd;
//      if(connections > 0) {
//        clients[connections - 1].buddy = connfd;
//        clients[connections].buddy = clients[connections - 1].fd;
//      }
      
      // Print amount of connections
      if(connections + 1 == 1) {
        printf("%d connection\n", connections + 1);
      } else {
        printf("%d connections\n", connections + 1);
      }
      
      if(connections + 1 > MAX_CONNECTIONS) {
        printf("Connection terminated -- max connections reached.");
        Close(connfd);
      }
      
      // Copy current connection amount
      // 1st client is connection 0 (0th index in clients array)
      int current_client = clientIndex;
      
      // Create thread for new client, pass in their connection file descriptor
      // The connection file descriptor - 4 is also the index of that client in the
      //    `clients` array
			if(pthread_create(&threads, NULL, client_process, &current_client)) {
				fprintf(stderr, "Error creating thread.\n");
				exit(1);
			}
      
      connections++;
		}
	}
	
	free(haddrp);
  free(clients);
	return 0;
}

// Basically a pipe; route read from send fd to receiver fd
void chat(struct client * sender, struct client * receiver) {
	long n, buf;
  // read encrypted longs from sent file descriptor
	while((n = read(sender->fd, &buf, sizeof(long))) != 0) {
    printf("received %ld from %d -> sent to %d\n", buf, sender->fd, sender->buddy);
    if(buf < 0) {
      Close(sender->fd);
      sender->fd = 0;
      write(receiver->fd, &buf, sizeof(long));
      break;
    }
    if(receiver->fd == 0) {
      break;
    }
    // send encrypted longs to receiver file descriptor
		write(receiver->fd, &buf, sizeof(long));
	}
  return;
}

void* client_process(void* arg) {
  // 'connection' is the client's index in the global 'clients' array on the server.
  // 'i' is the chat flag. i = 0: no buddy conencted.
  int connection = *(int*) arg, i = 0;
  
  // Create another pointer to client struct using connection fd
	struct client* client = &clients[connection];
  
  // Initialize client's internal key for reading
  client->public = (struct key*) malloc(sizeof(struct key));
  
  // String buf to read incoming string data
  char buf[MAX_MESSAGE_SIZE];
  
  // Read client's username and load into struct
  // This could be utilized for additional features later (i.e. buddy list, who's online)
  read(client->fd, buf, MAX_MESSAGE_SIZE);
  strncpy(client->username, buf, 16);
  
  // Start main wrapper loop
  while(1) {
    // While client doesn't have a buddy, do nothing
    // This probably isn't the best method...
    if(client->buddy != 0) {
      i = 1;
      write(client->fd, &i, sizeof(int));
      // read in client's public key
      read(client->fd, client->public, sizeof(struct key));
      // tell buddy current client's public key
      write(client->buddy, client->public, sizeof(struct key));
      // initiate chat
      chat(client, find_client_by_fd(client->buddy));
      if(client->fd == 0) {
        printf("Logging %s out of chat\n", client->username);
        break;
      }
    }
  }
  // handle one buddy leaving.
  //Close(client->fd);
  
  // cleanup
  //client->fd = 0;
  struct client* buddy = find_client_by_fd(client->buddy);
  buddy->buddy = 0;
  client->buddy = 0;
  bzero(client->username, strlen(client->username) + 1);
  client->public->a = 0;
  client->public->b = 0;
  free(client->public);
  connections--;
  printf("Connection closed.\n");
	pthread_exit(NULL);
}

struct client* find_client_by_fd(int fd) {
  int i;
  for(i = 0; i < MAX_CONNECTIONS; i++) {
    if(clients[i].fd == fd) {
      return &clients[i];
    }
  }
  return NULL;
}