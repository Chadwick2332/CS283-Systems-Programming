#include <stdio.h>
#include <math.h>
#include "crypt.h"

void generate_keys(struct key * public, struct key * private) {
  long n, k;
  long p1, p2, c, m, e, d;
	printf("Enter the nth prime and the mth prime to compute\n");
	scanf("%ld %ld", &n, &k);
  getchar();
  p1 = getNthPrime(n);
  p2 = getNthPrime(k);
  c = p1 * p2;
  m = totient(p1) * totient(p2);
  e = coprime(m);
  d = mod_inverse(e, totient(m) - 1, m);
  public->a = e;
  public->b = c;
  private->a = d;
  private->b = c;
}

void generate_from_primes(int n, int m, struct key * public, struct key * private) {
  long p1, p2, c, t, e, d;
  p1 = getNthPrime((long) n);
  p2 = getNthPrime((long) m);
  c = p1 * p2;
  t = totient(p1) * totient(p2);
  e = coprime(t);
  d = mod_inverse(e, totient(t) - 1, t);
  public->a = e;
  public->b = c;
  private->a = d;
  private->b = c;
}

long coprime(long x) {
	long search = (x * 1.5) + 1;
  while(GCD(x, search) != 1) {
    search++;
  }
  return search;
}

long endecrypt(long msg_or_cipher, long key, long c) {
	return modulo(msg_or_cipher, key, c);
}

long GCD(long a, long b) {
	if(b) {
		return GCD(b, a % b);
	} else {
		return a < 0 ? -a : a;
	}
}

long mod_inverse(long base, long exp, long m) {
  return modulo(base, exp, m);
}

// long modulo(long a, long b, long c) {
// 	if(b == 0) {
//     return 1;
//   }
//   if(b == 1) {
//     return a % c;
//   } else {
//     return (a * modulo(a, b - 1, c)) % c;
//   }
// }
// 

long modulo(long a, long b, long c) {
	long t = 1, eprime = 1;
	for(; eprime < b + 1; eprime++) {
		t = (t * a) % c;
	}
	return t;
}

long totient(long n) {
	double prod = n;
  long i = 2;
  for(; i < n + 1; i++) {
    if((long) n % i == 0 && isPrime(i)) {
      prod *= 1 - (1.0 / i);
    }
  }
  return (long) prod;
}

// brute force is least complicated
long getNthPrime(long n) {
	if(n <= 0) {
		return 0;
	}
  if(n == 1) {
    return 2;
  }
	long count = 1, check = 3;
	while(1) {
		if(isPrime(check)) {
			count++;
		}
    if(count == n) {
			return check;
		}
		check++;
	}
}

int isPrime(long n) {
	if(n <= 0) {
		return 0;
	}
	if(n % 2 == 0) {
		return (n == 2) ? 1 : 0;
	}
	else if(n % 3 == 0) {
		return (n == 3) ? 1 : 0;
	}
	double dn = n;
	int step = 4, max = (int) sqrt(dn) + 1, i;
	for(i = 5; i < max; i += 2) {
		if(n % i == 0) {
			return 0;
		}
	}
	return 1;
}