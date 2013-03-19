#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void sort(int* a, int size);

int main (int argc, char const *argv[])
{
  int i;
  srand(time(NULL));
  
  int size = 10;
  int* a = (int*) malloc(sizeof(int) * size);
  
  for(i = 0; i < size; i++) {
    a[i] = rand() % 20;
    printf("%d ", a[i]);
  }
  sort(a, size);
  printf("\n%s\n", "Sorting...\0");
  for(i = 0; i < size; i++) {
    printf("%d ", a[i]);
  }
  printf("\n");  
  free(a);
  return 0;
}

// Selection sort
void sort(int* a, int size) {
  int i, j, t, min;
  for(i = 0; i < size; i++) {
    min = i;
    for(j = i+1; j < size; j++) {
      if(a[j] < a[min]) {
        min = j;
      }
    }
    if(min != j) {
      t = a[i];
      a[i] = a[min];
      a[min] = t;
    }
  }
}