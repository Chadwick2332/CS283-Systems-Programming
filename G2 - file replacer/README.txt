Christian Benincasa
CS283 - Assignment 2 - File Renamer
2/12/13

This program is meant to scan a directory and rename files and folders based on a pattern. In addition, the program can scan each file that matches a certain pattern and change the text within these files based on pattern matching.

This program was tested on OSX 10.8.2 as well as the Drexel's Tux machines. The program was compiled with GCC and tested extensively in XCode. 

This program is run with certain options from the command line. These options are explained below:

	-d	: name of the base directory to start the recursive search [defaults to cwd]
	-p	: pattern to match
	-r	: replacement string
	-f	: restrict search to only files/folders containing this string
	-m	: mode -> 
			0	: rename files/folders
			1	: scan file contents for matches
					
