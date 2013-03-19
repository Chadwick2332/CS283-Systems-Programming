#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char const *argv[])
{
  int i;
  char** a = (char**) malloc(sizeof(char*) * 10);
  for(i = 0; i < 10; i++) {
    a[i] = (char*) malloc(sizeof(char) * 15);
    strncpy(a[i], "Christian", strlen("Christian") + 1);
  }

  for(i = 0; i < 10; i++) {
    printf("%s\n", a[i]);
  }

  for(i = 0; i < 10; i++) {
    free(a[i]);
  }
  
  free(a);
  return 0;
}