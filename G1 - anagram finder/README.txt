Christian Benincasa
CS283 - Systems Programming 
Assignment 1 - Anagram Solver
After implementing the hash table and basic utility functions, I decided to try the optimization technique by creating an array of hash tables organized by word length. Implementing this didn't go as easily as I expected, but after a lot of work finding potential problems with memory management I got everything working successfully. I even noticed a huge jump in performance when finding anagrams.

Three utility functions were used to aid in program. One was a string replace function. The implementation was found on StackOverflow (link in code) and modified for readability and style. This function was used to replace characters in a string; specifically to replace any newline characters with null characters for normalization for comparison. A string sort function was also written in order to compare strings more easily. The algorithm used was selection sort. Lastly, the print_list function was written to print all of the contents of a particular linked list (Bucket). This was useful for debugging purposes.

I found that the anagram solver was easily ported to include functionality for the “scrabble solver”. This was implemented by adding two additional parameters to the generate_anagrams function: a letter (char) and a position (int). These are defined on the command line and if they aren’t defined are set to the null character and -1, respectively. When set to those values, the character and position are ignored.

After implementing the scrabble solver, I saw the opportunity to modify further and include functionality for finding all words contained within a set of letters. The words generated range from length 2 to the length of the set. I did this to add more functionality and for a bit of additional challenge.
The program was tested using the GCC compiler, GDB debugger, and Valgrind. The program was tested on Drexel’s tux systems as well as my local machine running Mac OS X 10.8.

To check for memory leaks, the program was run through Valgrind on Tux using this command: valgrind --leak-check=full --show-reachable=yes ./main.o begin

Sample runs:
./main.o anagram begin
Finding anagrams of ‘begin’... binge being

./main.o anagram begin g 4
Finding anagrams of ‘begin’ with ‘g’ at position 4... being

./main.o permutations begin
All words contained in 'begin'... 2 letter words
be ge ge ie gi ne en ni in

3 letter words
beg gib gib big neb gie ben ben nib bin gen nei nig ing ing gin

4 letter words
gibe beng inbe bine bien beni beni bing gien gein

5 letter words binge being begin

Makefile commands:
make run: run program with a default word.
make build: compile the program
WORD=myword make anagram: find all anagrams of myword
WORD=myword LETTER=o PLACE=2 make scrabble: find all anagrams of myword with ‘o’ in position 2
WORD=myword make permutations: find all words contained within myword WORD=myword LETTER=o PLACE=2 make position-permutations: find all words contained within myword with letter ‘o’ at position 2.