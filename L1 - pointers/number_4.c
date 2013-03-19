#include <stdlib.h>
#include <stdio.h>
#include <time.h>

struct ListNode
{
  int data;
  struct ListNode* next;
} node;

void list_sort(struct ListNode* list);
void append(struct ListNode *first, struct ListNode *append);
void print_list(struct ListNode *first);

int main(int argc, char const *argv[])
{
  int i, data;
  srand(time(NULL));
  int size = 10;

  // Initialize head of linked list.
  struct ListNode* first = (struct ListNode*) malloc(sizeof(struct ListNode));
  data = rand() % 10;
  first->data = data;
  first->next = NULL;
  
  // Fill linked list
  for(i = 1; i < size; i++) {
    data = rand() % 10;
    struct ListNode* newNode = (struct ListNode*) malloc(sizeof(struct ListNode));
    newNode->data = data;
    newNode->next = NULL;
    append(first, newNode);
  }

  print_list(first);
  printf("Sorting...\n");
  list_sort(first);
  print_list(first);

  // Free Linked List
  struct ListNode* temp = (struct ListNode*) malloc(sizeof(struct ListNode));
  temp = first->next;
  while(temp != NULL) {
    first->next = temp->next;
    temp->next = NULL;
    free(temp);
    temp = first->next;
  }
  free(first);
  return 0;
}

// Selection sort modified to use pointers
// Inspired by example http://en.wikipedia.org/wiki/Selection_sort
void list_sort(struct ListNode *first) {
  int t;
  struct ListNode *iter, *iter_2, *min;
  for(iter = first; iter != NULL; iter = iter->next) {
    min = iter;
    for(iter_2 = iter->next; iter_2 != NULL; iter_2 = iter_2->next) {
      if(iter_2->data < min->data) {
        min = iter_2;
      }
    }
    if(min != iter) {
      t = iter->data;
      iter->data = min->data;
      min->data = t;
    }
  }
}

void append(struct ListNode *first, struct ListNode *append) {
  struct ListNode *current = first;
  while(current->next != NULL) {
    current = current->next;
  }
  current->next = append;
}

void print_list(struct ListNode *first) {
  struct ListNode *current = first;
  while(current != NULL) {
    printf("%d ", current->data);
    current = current->next;
  }
  printf("\n");
}