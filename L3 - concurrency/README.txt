Christian Benincasa
CS283 - Lab 3
Concurrent Programming
2/20/13

This program demonstrates how mutual exclusion locks can be used to eliminate race conditions from threaded programs. In addition, this program also shows how the placement of mutex locks can greatly affect the execution time of the program. The programs demonstrate: 1. how a lack of mutex locks will let the program succumb to the race condition and produce inaccurate output, 2. how mutex locks within the long loop will cause a major slowdown of the program, and 3. how mutex locks around the loop will cause accurate output with no slowdown in comparison to no locks.

The following times are from tests on my local machine (OS X 10.8)

Average runtime for program without mutex locks: 								0.41s
Average runtime for program with mutex lock around increment: 	3.31s
Average runtime for program with mutex lock around loop: 				0.40s

Logs of all three of these test are included in this directory.

Included are the source files for these three tests. They can be built and ran using the included Makefile. To compile all of the programs, use the `make build` command. To run all of the tests, use `make runall`. You can run each test individually using `make run_no_locks`, `make run_locks`, `make run_locks_loop` to run the test with no locks, the test with locks around the increment, and the test with locks around the loop, respectively. Clean the directory using `make clean`.
