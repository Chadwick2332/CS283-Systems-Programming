Christian Benincasa
CS283 - Lab 7
Thread Barriers & Locks
3/11/13

This assignment tasked us with creating programs including a concurrent linked list, a thread barrier library, and a solution to the Dining Philosophers problem. These programs were each tested on my local machine running OSX 10.8.2 as well as on Drexel's Tux servers. 

Notes on Linked List thread-safety:

If thread-safety didn't exist for the Linked List program, insertions and deletions with the list could easily become corrupt. A crude example of why linked list operations are not thread-safe can be seen below (each line represents a context switch in the program).

	Thread 1 finds the node to delete
	Thread 2 deletes the node after the node Thread 1 wants to delete
	Thread 2 doesn't reroute pointers of previous node
	Thread 1 tries to point to the next node, but it is NULL
	
Printing the list is also not thread-safe. If a node is deleted but then the context switches back to the print statement before the pointers can be rearranged, the printList() function will end up with a NULL pointer exception. These threading problems can be quelled by adding a mutex attribute to the Node struct. This would mean that each linked list has its own mutex and can lock on non-thread-safe functions to ensure safety.

Notes on thread barrier program:

The source for the thread barrier library can be found in src/barrier.c. The library exists as a static library archive (libbarrier.a) in the lib/ subdirectory. The library is archived and linked within the included Makefile.

The programs can all be compiled using the included Makefile. `make all` will compile all of the files. `make run` will run all of the programs. `make run_linked_list`, `make run_philosophers`, and `make run_barrier` will run the linked lists, dining philosophers, and barrier demonstrations, respectively. `make clean` removes all of the object files generated from compilation.