Christian Benincasa
CS283 - Systems Programming
Lab 1 - Pointers and Dynamic Memory


This lab was meant to give us practice with dynamic memory allocation and pointers in C. The lab consisted of various exercises including sorting via pointer arithmetic and creating dynamically allocated arrays.


Each program in this lab was compiled using GCC and tested in Unix environments, including Tux. Program compilation and running can be duplicated using the included Makefile. To compile and run all of the included programs, simply run ‘make’ in the directory. To compile and run individual programs, run ‘make problem_1’ (or problem_2, etc.). To clean up the directory and deleted generated .o files, run ‘make clean’.


Programs were tested on two systems: my local system running Mac OS X 10.8.2 and remotely on Drexel’s Tux system. Testing on both systems yielded the expected results. 


Specifically, for problem 5 regarding the allocation of memory for growing arrays, I discovered that it was more efficient to double the size of the array when the limit was reached instead of allocating only enough space for one additional element. The program that doubles array size runs over 8x faster than its counterpart.


I think that this lab was extremely helpful in some basic practice in dynamic allocation and pointer arithmetic. I was a bit uncomfortable with pointers before this after not having used them in several terms, but after this lab I feel that I have more of a grasp on the practice.