#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "crypt.h"

int main (int argc, char const *argv[])
{
	if(argc < 4) {
		fprintf(stderr, "Insufficient argument count.\n");
	}
	long n = (long) atoi(argv[1]);
	long m = (long) atoi(argv[2]);
	int mode = atoi(argv[3]);
	if(mode == 0) {
	 	struct key* public = (struct key*) malloc(sizeof(struct key));
		struct key* private = (struct key*) malloc(sizeof(struct key));
	
		generate_from_primes(n, m, public, private);
	
		printf("Public key: (%ld, %ld)\nPrivate key: (%ld, %ld)\n", public->a, public->b, private->a, 			private->b);	
	} else {
		long i, j, d, start = 2;
		long original = 13, test;
		long crypted = endecrypt(original, n, m);
		char test_string[10] = "Christian\0";
		// let's figure out d...
		while(start < LONG_MAX) {
			for(i = start; i < LONG_MAX; i++) {
				test = endecrypt(crypted, i, m);
				if(test == original) {
					d = i;
					printf("Found Possible Private key: (%ld, %ld)\n", d, m);
					break;
				}
			}
			printf("Test encryption. Encrypting '%s'...\n\n", test_string);
			long string_crypt, string_crypt_reverse;
			for(j = 0; j < 10; j++) {
				if(test_string[j] != '\0') {
					string_crypt = endecrypt((long) test_string[j], n, m);
					printf("encrypted: %ld\n", string_crypt);
					string_crypt_reverse = endecrypt(string_crypt, i, m);
					printf("decrypted: %ld\n", string_crypt_reverse);
					if(string_crypt_reverse == (long) test_string[j]) {
						printf("character: %c\n",(char) string_crypt_reverse);
						printf("\n");
					} else {
						printf("Character doesn't match. Starting loop again.\n");
						start = d + 1;
					}
				} else {
					// we've reached the end of the string and everything matches!
					printf("Private key (%ld, %ld) is valid\n\n", d, m);
					return 0;
				}
			}
		}
	}
 	
	return 0;
}