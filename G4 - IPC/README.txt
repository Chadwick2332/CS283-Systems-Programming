Christian Benincasa
Assignment 4 - Connect Four - IPC
3/6/13

This program was about IPC by using fork() and pipe() to have a parent play M simultaneous games of Connect 4. 

This program was tested on Tux mainly, with optimal results. Compiled with GCC. The program was tested with varied parameters including different board dimensions and simultaneous game amounts. I've included several logs of test runs in this zip.

The Makefile includes the specified targets to build, run, test, and clean the program.

This program was initially extremely difficult and complicated for me to get working. I had a program that had varying results and hung on some blocking I/O functions. I decided to redesign and go with the select() function for easier access and handling of the I/O functions and it really saved me from a lot of headache.