#ifndef __CHAT_CRYPT__
#define __CHAT_CRYPT__

struct key {
	long a;
  long b;
};
void generate_keys(struct key * public, struct key * private);
void generate_from_primes(int n, int m, struct key * public, struct key * private);
long coprime(long x);
long endecrypt(long msg_or_cipher, long key, long c);
long GCD(long a, long b);
long mod_inverse(long base, long exp, long m);
long modulo(long a, long b, long c);
long totient(long n);
long getNthPrime(long n);
int isPrime(long n);

#endif /* __CHAT_CRYPT__ */