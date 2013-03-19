#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

// Concuurent Linked List node
struct node_t;
typedef	struct node_t Node;
struct node_t {
	int value;
	Node* next;
	pthread_mutex_t* lock;
};

// Argument struct for passing information to threads
typedef struct thread_args {
	Node** first;
	int thread;
} args_t;

/** Print linked list **/
void printList(Node* first) {
	if(first != NULL) {
		Node* iter;
		pthread_mutex_lock(first->lock);
		for(iter = first; iter != NULL; iter = iter->next) {
			printf("%d ", iter->value);
		}
		printf("\n");
		pthread_mutex_unlock(first->lock);
	}
}

/** Insert after specified node **/
void insert(Node* previous, Node* new) {
	pthread_mutex_lock(previous->lock);
	if(previous != NULL && new != NULL) {
		new->next = previous->next;
		previous->next = new;
	}
	pthread_mutex_unlock(previous->lock);
}

/** Append to end of list **/
void append(Node* first, Node* new) {
	pthread_mutex_lock(first->lock);
	if(first != NULL && new != NULL) {
		Node* iter = first;
		while(iter->next != NULL) iter = iter->next;
		iter->next = new;
		printf("Thread[%ld] appended %d to %d\n", (unsigned long) pthread_self(), new->value, iter->value);
	};
	pthread_mutex_unlock(first->lock);
}

/** This function should be prefered over append() for users
	* Creates node with specified value and appends to end of LL **/
void appendWithValue(Node* anchor, int value) {
	Node* new = (Node*) malloc(sizeof(Node));
	if(new) {
		new->value = value;
		new->next = NULL;
		new->lock = anchor->lock;
		append(anchor, new);
	} else {
		fprintf(stderr, "Error allocating memory\n");
		exit(1);
	}
}

/** Find a node by a specific value and delete it **/
void deleteNodeWithValue(Node* first, int val) {
	pthread_mutex_lock(first->lock);
	if(first != NULL) {
		Node* iter, *previous = NULL;
		for(iter = first; iter != NULL; previous = iter, iter = iter->next) {
			if(iter->value == val) {
				printf("Thread[%ld] deleted %d\n", (unsigned long) pthread_self(), val);
				previous->next = iter->next;
				free(iter);
			}
		}
	}
	pthread_mutex_unlock(first->lock);
}

Node* alloc(int value) {
	Node* new = (Node*) malloc(sizeof(Node));
	if(new) {
		new->value = value;
		new->next = NULL;
		new->lock = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
		pthread_mutex_init(new->lock, NULL);
		return new;
	} else {
		return NULL;
	}
}

void dealloc(Node* first) {
	pthread_mutex_destroy(first->lock);
	Node* iter = first, *temp;
	while(iter != NULL) {
		temp = iter;
		iter = iter->next;
		free(temp);
	}
	first = NULL;
}

void* thread_routine(void* arg) {
	struct thread_args* args = (struct thread_args*) arg;
	Node* anchor = *(args->first);
	int i;
	for(i = 2; i < 10; i++) {
		appendWithValue(anchor, i);
	}
	printList(anchor);
	pthread_exit(0);
}

int main (int argc, char const *argv[])
{
	int i;
	Node* a = alloc(1);
	if(a) {
		pthread_t threads[2];
		args_t* args = (args_t*) malloc(sizeof(args_t));
		args->first = &a;
		for(i = 0; i < 2; i++) {
			pthread_create(&threads[i], NULL, thread_routine, args);
		}
	
		for(i = 0; i < 2; i++) {
			pthread_join(threads[i], NULL);
		}

		dealloc(a);
		
	} else {
		fprintf(stderr, "Error allocating memory for new linked list\n");
		exit(1);
	}
	
	return 0;
}