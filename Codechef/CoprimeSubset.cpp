/* assumes int, long are 32, 64 bits respectively */
#include <stdio.h>
#include <stdlib.h>

enum {
  NMAX = 3000
};

static int n;
static long m;
static unsigned smallfactors[NMAX + 1];
static int prime[NMAX - 1];
static int primecount;
static int smallprimecount;
static int largeprimefactor[NMAX + 1];
static int largeprimecount[NMAX + 1];
static long **table;

static void eratosthenes(void) {
  int i;
  for (i = 2; i * i <= n; i++) {
    int j;
    if (smallfactors[i]) continue;
    for (j = i; j <= n; j += i) smallfactors[j] |= 1U << primecount;
    prime[primecount++] = i;
  }
  smallprimecount = primecount;
  for (; i <= n; i++) {
    if (!smallfactors[i]) prime[primecount++] = i;
  }
  if (0) {
    int k;
    for (k = 0; k < primecount; k++) printf("%d\n", prime[k]);
  }
}

static void makelargeprimefactor(void) {
  int i;
  for (i = smallprimecount; i < primecount; i++) {
    int p = prime[i];
    int j;
    for (j = p; j <= n; j += p) largeprimefactor[j] = p;
  }
}

static void makelargeprimecount(void) {
  int i = 1;
  int j;
  for (j = primecount; j > smallprimecount; j--) {
    for (; i <= n / prime[j - 1]; i++) {
      largeprimecount[i] = j - smallprimecount;
    }
  }
  if (0) {
    for (i = 1; i <= n; i++) printf("%d %d\n", i, largeprimecount[i]);
  }
}

static void maketable(void) {
  int i;
  int j;
  table = calloc(smallprimecount + 1, sizeof *table);
  for (i = 0; i <= smallprimecount; i++) {
    table[i] = calloc(1U << smallprimecount, sizeof *table[i]);
  }
  table[0][0U] = 1L % m;
  for (j = n; j >= 2; j--) {
    int lpc = largeprimecount[j];
    unsigned sf = smallfactors[j];
    if (largeprimefactor[j]) continue;
    for (i = 0; i < smallprimecount; i++) {
      long *cur = table[i];
      long *next = table[i + 1];
      unsigned f;
      for (f = sf; f < (1U << smallprimecount); f = (f + 1U) | sf) {
        cur[f] = (cur[f] + cur[f & ~sf]) % m;
      }
      if (lpc - i <= 0) continue;
      for (f = sf; f < (1U << smallprimecount); f = (f + 1U) | sf) {
        next[f] = (next[f] + cur[f & ~sf] * (lpc - i)) % m;
      }
    }
  }
}

static long timesexp2mod(long x, int y) {
  long z = 2L % m;
  for (; y > 0; y >>= 1) {
    if (y & 1) x = (x * z) % m;
    z = (z * z) % m;
  }
  return x;
}

static long computetotal(void) {
  long total = 0L;
  int i;
  for (i = 0; i <= smallprimecount; i++) {
    unsigned f;
    for (f = 0U; f < (1U << smallprimecount); f++) {
      total = (total + timesexp2mod(table[i][f], largeprimecount[1] - i + 1)) % m;
    }
  }
  return total;
}

int main(void) {
  scanf("%d%ld", &n, &m);
  eratosthenes();
  makelargeprimefactor();
  makelargeprimecount();
  maketable();
  if (0) {
    int i;
    for (i = 0; i < 100; i++) {
      printf("%d %ld\n", i, timesexp2mod(1L, i));
    }
  }
  printf("%ld\n", computetotal());
  return EXIT_SUCCESS;
}