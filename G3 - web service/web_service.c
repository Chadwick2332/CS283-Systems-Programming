//
//  main.c
//  web_service
//
//  Created by Christian Benincasa on 2/25/13.
//  Copyright (c) 2013 Christian Benincasa. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "csapp.h"
#include "stringly.h"
#include "time.h"

// Constant strings used for constructing responses
const char* GENERAL_STATUS = "HTTP/1.1 %d %s\n";
const char* GENERAL_MIME = "Content-Type: %s";
const char* ERROR_HTML = "<html><head></head><body><h1>PAGE NOT FOUND</h1></body></html>";
const char* HTML_WITH_HEADER = "<html><head></head><body><h1>%s</h1></body></html>";

// Struct used for passing extra header params when constructing response
// `size` used for easy interation
// `headers` is an array of header parameters
struct header_options {
  int size;
  char** headers;
};

void* serve(void* arg);
char* build_response(char* content, char* content_type, int res_code, struct header_options* opts);
char* parse_request(char* request);
char* watch(char* params);
char* echo(char* params);
char* bananas();
char* fibonacci(char* params);
long long unsigned fib();

int main(int argc, const char * argv[])
{
  int listenfd, connfd, port = 4567;
  socklen_t clientlen;
  struct sockaddr_in clientaddr;
  pthread_t threads;
  if(argc > 1) {
    port = atoi(argv[1]);
  }
  
  listenfd = open_listenfd(port);
  if(listenfd < 0) {
    fprintf(stderr, "Error while opening listening port for server.\n");
    exit(1);
  }
  
  printf("Server running on port %d\n", port);
  while(1) {
    clientlen = sizeof(clientaddr);
    connfd = Accept(listenfd, (SA*) &clientaddr, &clientlen);
    if(connfd == -1) {
      fprintf(stderr, "Error establishing connectiong with client %s.\n", inet_ntoa(clientaddr.sin_addr));
    } else {
      printf("Serving %s.\n", inet_ntoa(clientaddr.sin_addr));
      pthread_create(&threads, NULL, serve, &connfd);
    }
  }
  
  return 0;
}

void* serve(void* arg) {
  int fd = *(int*) arg;
  char request[2048];
  // Read in HTTP request
  read(fd, &request, 2048);
  char* response = parse_request(request);
  // Write HTTP response
  write(fd, response, strlen(response));
  free(response);
  close(fd);
  pthread_exit(0);
}

char* build_response(char* content, char* content_type, int res_code, struct header_options* opts) {
  char* headers = (char*) malloc(2048);
  char* response = (char*) malloc(2048);
  char* temp = (char*) malloc(128);
  char* content_type_string = (char*) malloc(128);
  // Construct response header based on response code
  if(res_code == 200) {
    snprintf(headers, 128, GENERAL_STATUS, res_code, "OK");
  }
  else if(res_code == 404) {
    snprintf(headers, 128, GENERAL_STATUS, res_code, "Not Found");
  }
  // Redirects must have Location headers so the browser knows where to go
  else if(res_code == 302) {
    snprintf(headers, 128, GENERAL_STATUS, res_code, "Found");
    // Pull the 'Host" header for the redirect to /watch
    if(opts) {
      int i;
      for(i = 0; i < opts->size; i++) {
        printf("%s\n", opts->headers[i]);
        char* tokens = strtok(opts->headers[i], " ");
        for(; tokens; tokens = strtok(NULL, " ")) {
          if(strncmp(tokens, "Host:", strlen(tokens)) == 0) {
            char* temp2 = (char*) malloc(128);
            tokens = strtok(NULL, " ");
            strncpy(temp2, tokens, 128);
            temp2 = str_replace(temp2, "\r", "");
            snprintf(temp, 128, "Location: http://%s/watch\n", temp2);
            free(temp2);
            break;
          }
        }
      }
      // Defaults to localhost and default port 4567
    } else {
      snprintf(temp, 128, "Location: http://%s/watch\n", "localhost:4567");
    }
    strncat(headers, temp, 128);
  }
  else {
    snprintf(headers, 128, GENERAL_STATUS, res_code, "OK");
  }
  // Construct Content-Type Header
  snprintf(temp, 128, GENERAL_MIME, content_type);
  // Append Content-Type to Header String
  strncat(headers, temp, strlen(temp));
  // Append Headers
  strncat(response, headers, strlen(headers));
  // Append Content-Type Header
  strcat(response, "\n\n");
  // If string length is greater than what's left in response, realloc to correct size
  if(strlen(content) > (2047 - strlen(response))) {
    char* new_response = realloc(response, strlen(response) + strlen(content) + 1);
    if(new_response) {
      response = new_response;
    }
  }
  // Append content to response
  strncat(response, content, strlen(content));
  free(temp);
  free(content_type_string);
  return response;
}

char* parse_request(char* request) {
  char* tokenized_request, *tokenized_params;
  char* req_ctx, *param_ctx;
  char* parameters = (char*) malloc(1024);
  char* response = (char*) malloc(2048);
  char* result = (char*) malloc(128);
  char* function_name = (char*) malloc(64);
  printf("%s\n", request);

  // Split HTTP Request by line
  tokenized_request = strtok_r(request, "\n", &req_ctx);
  printf("Request: %s\n\n", tokenized_request);
  
  // Split each header by whitespace
  tokenized_params = strtok_r(tokenized_request, " ", &param_ctx);
  if(strncmp(tokenized_params, "GET", 3) == 0) {
    // process GET request
    
    // Get requested resource name from first line of request
    tokenized_params = strtok_r(NULL, " ", &param_ctx);
    strncpy(function_name, tokenized_params, 64);
    
    // Replace leading slash for easier comparison
    // This slash would be useful if this server served files
    function_name = str_replace(function_name, "/", "");
    
    // Check if parameters string is present
    function_name = strtok_r(function_name, "?", &param_ctx);
    parameters = strtok_r(NULL, "?", &param_ctx);
    
    if(function_name) {
      if(strncmp(function_name, "watch", strlen(function_name)) == 0) {
        if(parameters == NULL) {
          result = watch(NULL);
        } else {
          result = watch(parameters);
        }
        response = build_response(result, "text/html", 200, NULL);
      }
      else if(strncmp(function_name, "bananas", strlen(function_name) + 1) == 0) {
        result = bananas();
        response = build_response(result, "text/plain", 200, NULL);
      }
      else if(strncmp(function_name, "fibonacci", strlen(function_name) + 1) == 0) {
        if(parameters == NULL) {
          result = fibonacci(NULL);
        } else {
          result = fibonacci(parameters);
        }
        response = build_response(result, "text/plain", 200, NULL);
      }
      else {
        response = build_response((char*)ERROR_HTML, "text/html", 404, NULL);
      }
      
    // Handle "GET /"
    // This condition constructs a 302 redirect HTTP response
    } else {
      struct header_options* opts = (struct header_options*) malloc(sizeof(struct header_options));
      opts->headers = (char**) malloc(sizeof(char*) * 10);
      opts->size = 0;
      tokenized_request = strtok_r(NULL, "\n", &req_ctx);
      char* temp = (char*) malloc(1024);
      while(tokenized_request) {
        strncpy(temp, tokenized_request, strlen(tokenized_request));
        tokenized_params = strtok_r(temp, " ", &param_ctx);
        if (strncmp(tokenized_params, "Host:", strlen(tokenized_params)) == 0) {
          opts->headers[opts->size] = malloc(strlen(tokenized_request) + 1);
          strncpy(opts->headers[opts->size], tokenized_request, strlen(tokenized_request));
          opts->size++;
        }
        tokenized_request = strtok_r(NULL, "\n", &req_ctx);
      }
      free(temp);
      response = build_response("", "text/plain", 302, opts);
      free(opts);
    }
  }
  // Handle POST requests
  else if(strncmp(tokenized_params, "POST", 4) == 0) {
    for (tokenized_request = strtok_r(NULL, "\n", &req_ctx); tokenized_request; tokenized_request = strtok_r(NULL, "\n", &req_ctx)) {
      if(strncmp(tokenized_request, "\r", strlen(tokenized_request)) == 0) {
        // Grab only POST parameters, not any headers
        strncpy(parameters, req_ctx, 1024);
        result = echo(parameters);
        response = build_response(result, "text/html", 200, NULL);
        free(parameters);
      }
    }
  }
  free(result);
  free(function_name);
  return response;
}

// Generate random number between 0 and 99
// Return message about bananas
char* bananas() {
  srand((unsigned int)time(NULL));
  char* my_bananas = (char*) malloc(64);
  snprintf(my_bananas, 64, "I HAVE %d BANANAS\n", rand() % 100);
  return my_bananas;
}

// Generate embedded youtube video
// URLs in the form http://<host>:<port>/watch[?w=<number>&h=<number>&vid=<string>
// Example (loads goat beatbox video -__-)
//    http://localhost:4567/watch?vid=TWXZy2dOuBc&w=853&h=480
char* watch(char* params) {
  char* vid = (char*) malloc(16);
  strncpy(vid, "5HI_xFQWiYU", 16);
  int w = 960, h = 720;
  char* param_ctx, *arg_ctx;
  char* embed_code = (char*) malloc(128);
  if(params) {
    char* tokens = strtok_r(params, "&", &param_ctx);
    for(; tokens; tokens = strtok_r(NULL, "&", &param_ctx)) {
      char* param = strtok_r(tokens, "=", &arg_ctx);
      char* arg = strtok_r(NULL, "=", &arg_ctx);
      if (strncmp(param, "w", strlen(param)) == 0) {
        w = atoi(arg);
      }
      else if(strncmp(param, "h", strlen(param)) == 0) {
        h = atoi(arg);
      }
      else if(strncmp(param, "vid", strlen(param)) == 0) {
        strncpy(vid, arg, 16);
      }
    }
  }
  snprintf(embed_code, 128, "<iframe width=\"%d\" height=\"%d\" src=\"http://www.youtube.com/embed/%s?autoplay=1\" frameborder=\"0\" allowfullscreen></iframe>\n", w, h, vid);
  free(vid);
  return embed_code;
}

// Calculate fibonacci number to n places
// Will overflow when n > 92
char* fibonacci(char* params) {
  long n = 25;
  unsigned long size = 1024;
  char* result_string = (char*) malloc(size);
  char* param_ctx, *arg_ctx;
  if(params) {
    char* tokens = strtok_r(params, "&", &param_ctx);
    for(; tokens; tokens = strtok_r(NULL, "&", &param_ctx)) {
      char* param = strtok_r(tokens, "=", &arg_ctx);
      char* arg = strtok_r(NULL, "=", &arg_ctx);
      if(strncmp(param, "n", strlen(param)) == 0) {
        n = atol(arg);
      }
    }
  }
  long i;
  for(i = 0; i < n + 1; i++) {
    if(strlen(result_string) >= (0.75 * size)) {
      strextend(&result_string, &size);
    }
    long long result = fib(i);
    if(result < 0) {
      break;
    }
    snprintf(result_string, size, "%s%ld:\t%lld\n", result_string, i, result);
  }
  for(; i < n + 1; i++) {
    if(strlen(result_string) >= (0.75 * size)) {
      strextend(&result_string, &size);
    }
    snprintf(result_string, size, "%s%ld:\t%s\n", result_string, i, "overflow");
  }
  
  return result_string;
}

// Compute nth fibonacci number iteratively
long long unsigned fib(long n) {
  long long unsigned last = 0, this = 1, new, i;
  if (n < 2) return n;
  for (i = 1 ; i < n ; ++i) {
    new = last + this;
    last = this;
    this = new;
  }
  return this;
}

// Used for POST requests
// Returns HTML page of value of "name" parameter wrappted in <h1> tags
char* echo(char* params) {
  char* message = (char*) malloc(128);
  char* name = (char*) malloc(64);
  strncpy(name, "Christian", 64);
  char* param_ctx, *arg_ctx;
  if(params) {
    char* tokens = strtok_r(params, "&", &param_ctx);
    for(; tokens; tokens = strtok_r(NULL, "&", &param_ctx)) {
      char* param = strtok_r(tokens, "=", &arg_ctx);
      char* arg = strtok_r(NULL, "=", &arg_ctx);
      if(strncmp(param, "name", strlen(param)) == 0) {
        strncpy(name, arg, strlen(message));
      }
    }
  }
  
  snprintf(message, 128, "My name is %s!\n", name);
  free(name);
  return message;
}