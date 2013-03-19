Christian Benincasa
CS283 - Shell
3/18/13

This assignment tasked us with adding the vital functionality to the provided tiny shell program, tsh.c. This functionality included job management and I/O redirection (<, >, and |). This program had to pass 16 regression tests that assessed how the shell handled signals including SIGINT, SIGTSTP, and SIGCHLD. 

This program was tested on my local machine running OSX 10.8.2 as well as Drexel's Tux servers. 

The hardest part of this program for me was the I/O redirection, especially the pipes. Figuring out how to look for valid redirection (that can be in any order) was tough. In addition, the task of opening pipes and creating processes for the programs was a challenge.

Overall, I had a lot of fun with this project. I actually think I'm going to continue adding functionality to this shell for additional practice!