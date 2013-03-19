#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  int i;
  int* a = (int*) malloc(sizeof(int) * 10);
  for(i = 0; i < 10; i++) {
    a[i] = i + 1;
    printf("%d\n", a[i]);
  }
  free(a);
  return 0;
}