#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
  int* array;
  int size;
  int set;
} Array;

void build_array(Array* a, int size);
void array_add(Array* a, int element);
void array_add_double(Array* a, int element);
void array_remove_by_index(Array* a, int index);
int array_get(Array* a, int index);
void print_array(Array* a);

int main(int argc, char const *argv[])
{
  const int ADD_AMOUNT = 1000000;
  int i, size;
  double start, stop;
  Array *a = (Array*) malloc(sizeof(Array));
  Array *b = (Array*) malloc(sizeof(Array));
  
  if(argv[1] != NULL) {
    size = atoi(argv[1]);
  } else {
    size = 10;
  }
  
  //Initialize arrays
  build_array(a, size);
  build_array(b, size);
  
  // Begin test 1 on Array a
  // Expanding array size by one when limit is reached
  start = clock();
  for(i = 0; i < ADD_AMOUNT; i++) {
    array_add(a, i);
  }
  stop = clock();
  printf("%10.2f : expanding array by one.\n", stop - start);
  
  // Begin test 2 on Array b
  // Doubling size of array when limit is reached
  start = clock();
  for(i = 0; i < ADD_AMOUNT; i++) {
    array_add_double(b, i);
  }
  stop = clock();
  printf("%10.2f : doubling size of array.\n", stop - start);
  
  // Free arrays and then structs
  free(a->array);
  free(b->array);
  free(a);
  free(b);
  return 0;
}

// Initialize array to specified size.
// Return Array*
void build_array(Array* a, int size) {
  if(a == NULL) {
    printf("Memory could not be allocated.\n");
    exit(1);
  }
  a->array = (int*) malloc(sizeof(int) * size);
  a->set = 0;
  a->size = size;
}

// Append element to the end of an Array
// If the set element count is the same as size, add space for 1 additional element
void array_add(Array* a, int element) {
  int i;
  if(a->set == a->size) {
    a->size++;
    a->array = realloc(a->array, sizeof(a->array) * a->size);
  }
  a->array[a->set++] = element;
}

// Testing out adding at certain index.
void array_add_at_index(Array* a, int index, int element) {
  if(index < a->size) {
    a->array[index] = element;
    a->set++;
  } else if(index > a->size) {
    a->array = realloc(a->array, sizeof(a->array) * index);
    a->array[index] = element;
    a->set++;
  }
}

// Append element to the end of an Array
// If the set element count is the same as the size, double allocated space
void array_add_double(Array* a, int element) {
  if(a->set == a->size) {
    a->size *= 2;
    a->array = realloc(a->array, sizeof(a->array) * (a->size * 2));
  }
  a->array[a->set++] = element;
}

// Remove an element by specifying the element's index
// Shift subsequent elements left
void array_remove_by_index(Array* a, int index) {
  if(index < a->size && index >= 0) {
    int i;
    for(i = index; i < (a->size); i++) {
      a->array[i] = a->array[i+1];
    }
    a->set--;
  } else {
    printf("Invalid index.\n");
  }
}

// Retrieve an element from an Array at a specified index.
int array_get(Array* a, int index) {
  if(index < a->size && index >= 0) {
    return a->array[index];
  } else {
    return -1;
  }
}

// Print an Array's elements and their respective indices
void print_array(Array* a) {
  int i;
  for(i = 0; i < a->size; i++) {
    printf("%d: %d\n", i, a->array[i]);
  }
  printf("\n");
}