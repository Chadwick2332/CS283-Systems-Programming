#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "csapp.h"
#include "crypt.h"
#include "stringly.h"

const int MAX_MESSAGE_SIZE = 2048;
const long EXIT_CHAT_SIGNAL = -100;
void* chat_read(void* args);
void* chat_write(void* args);

struct client {
	struct key* public;
  struct key* private;
  struct key* buddy_public;
  int fd;
  char username[16];
  char buddy_username[16];
};

int main (int argc, char ** argv)
{
  // CLIENT TIME
	struct client* me = (struct client*) Malloc(sizeof(struct client));
  me->public = (struct key*) Malloc(sizeof(struct key));
  me->private = (struct key*) Malloc(sizeof(struct key));
  me->buddy_public = (struct key*) Malloc(sizeof(struct key));
  char* host = (char*) Malloc(sizeof(char) * strlen(argv[1]) + 1);
  char* username = (char*) Malloc(sizeof(char) * 16);
  pthread_t threads[2];
  int clientfd, port = 4567;
  int k, e = 0, c = 0, d = 0, dc = 0;

  while((k = getopt(argc, argv, "s:p::E::C::D::d::")) != -1) {
    switch(k) {
      case 's':
        host = optarg;
        break;
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
      case '?':
        if(optopt == 's') {
          fprintf(stderr, "Option -p is required\n");
        } else {
          fprintf(stderr, "Unknown error! Try again.\n");
        }
        return 1;
    }
  }

  if(e == 0 && c == 0 && d == 0 && dc == 0) {
    generate_keys(me->public, me->private);
  } else {
    me->public->a = e;
    me->public->b = c;
    me->private->a = d;
    me->private->b = dc;
  }

  // Get username
  printf("Choose a username (max 16 characters): ");
  Fgets(username, 16, stdin);
  username = sanitize(username);
  strncpy(me->username, username, 16);
  
  // Connect to chat server
  clientfd = Open_clientfd(host, port);
  
	if(clientfd < 0) {
		fprintf(stderr, "Error connecting client\n");
	} else {
    int connected = 0;
    // Set client local file descriptor
		me->fd = clientfd;
    
    printf("Logged on as %s\n", username);
    // Send username to server
    write(me->fd, me->username, strlen(me->username) + 1);
    
    // Wait for buddy to connect; read() is blocking
    read(me->fd, &connected, sizeof(int));
    
    // Once connected, send client public key to server
    write(me->fd, me->public, sizeof(struct key));
    
    // Read back buddy's public key
    read(me->fd, me->buddy_public, sizeof(struct key));
  
    printf("Now connected.\n");
    
    // Create read and write threads
    if(pthread_create(&threads[0], NULL, chat_write, me)) {
      fprintf(stderr, "Threading error.");
      exit(1);
    }

    if(pthread_create(&threads[1], NULL, chat_read, me)) {
      fprintf(stderr, "Threading error.");
    }
    
    // Wait for write thread to terminate
    pthread_join(threads[0], NULL);
	}
  
  free(username);
  free(me);
	return 0;
}

void* chat_write(void* args) {
  int i;
  long crypt;
  struct client* me = (struct client*) args;
  char buf[MAX_MESSAGE_SIZE];
  while(1) {
    Fgets(buf, MAX_MESSAGE_SIZE, stdin);
    if(strncmp(buf, ".quit\n", strlen(buf)) == 0) {
      write(me->fd, &EXIT_CHAT_SIGNAL, sizeof(long));
      printf("Logging off...\n");
      break;
    } else {
      for(i = 0; i < strlen(buf); i++) {
        crypt = endecrypt(buf[i], me->private->a, me->private->b);
        Rio_writen(me->fd, &crypt, sizeof(long));
      }
    }
  }
  pthread_exit(NULL);
}

void* chat_read(void* args) {
	struct client* me = (struct client*) args;
  long buf, crypt;
  char part;
  char final_word[MAX_MESSAGE_SIZE];
	while(1) {
    while(read(me->fd, &buf, sizeof(long)) > 0) {
      crypt = endecrypt(buf, me->buddy_public->a, me->buddy_public->b);
      part = (char) crypt;
      final_word[strlen(final_word)] = part;
      final_word[strlen(final_word) + 1] = '\0';
      if(part == '\n') {
        printf("> %s", final_word);
        bzero(final_word, MAX_MESSAGE_SIZE);
      }
    }
	}
  pthread_exit(NULL);
}
