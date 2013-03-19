Christian Benincasa
CS283 - Lab 2

This lab focused on syncing two directories with one another. A slave directory should be synced with a master directory where any contents of the master directory not present in the slave directory should be copied over, any contents of the slave directory not present in the master directory should be deleted. If the directories both contain a certain file, the more recent one should be copied to the opposite directory.

This program was built and tested on two separate systems. The first system was my local machine using gcc v4.2.1 (Based on Apple Inc. build 5658) (LLVM build 2336.11.00). The machine is running OS X v10.8.2. The second machine was the Tux servers hosted by Drexel. 

There are several ways to compile and run the included sync.c program. The bin/ directory includes scripts to either quickly run and build, or just quickly run the program. These quick run scripts unzip a test environment and run the built program on the unzipped directories.

The other way to run the program is using the Makefile. The Makefile is more flexible because it takes in the environment variables DIR1 (master dir) and DIR (slave dir) and can execute the script on those directories. This is performed by typing this command:

	DIR1=./a DIR=./b make sync
	
The test environment can also be used by invoking the `test` target of the Makefile.

The program itself recursively traverses the master directory, appropriately syncing the slave directory and then runs through the slave directory, further synchronizing it with the master directory. The program includes some options (that cannot be toggled via the command line at this time) to aid in its usage. One option is the "verbose" option. This option is on by default. It will print out each operation performed by the program. The program also included a "dry run" option, which prints out what the program will do without actually executing any creation, copying, or deletion. Command line parsing for turning these options on and off after compilation is not implemented at this time. 

The program was tested using the test_area.zip file included. This zip file contains two directories (each with subdirectories) that covers each case (file in a, but not in b; file in b, but not in a; file in both). The program was tested repeatedly on both my local machine and Tux.