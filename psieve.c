#include <stdio.h>
#include <math.h>
#include "bspedupack.h"

unsigned long N = 100;
size_t P = 2;

void sieve() {
  bsp_begin(P);

  size_t core = bsp_pid();

  // Distribution: proc 0 gets 0, 1, ..., sqrt(N)
  // proc 1, ..., p get the remainders evenly

  unsigned long sizes[P];
  unsigned long startsAt[P];

  sizes[0] = (unsigned long) sqrt(N) + 1;
  startsAt[0] = 0;

  unsigned long size = (N - sizes[0] + 1) / (P - 1);
  unsigned long remains = (N - sizes[0] + 1) % (P - 1);

  for (size_t index = 1; index < P; ++index)
  {
    sizes[index] = size;

    if (index <= remains)
      ++sizes[index];

    startsAt[index] = startsAt[index - 1] + sizes[index - 1];
  }

  if(core == 0)
    for(size_t p = 0; p < P; ++p)
      printf("Core %zu gets %lu to %lu\n", p, startsAt[p], startsAt[p]+sizes[p]-1);

  int x[sizes[core]];
  unsigned long prime = 2;

  bsp_push_reg(&prime, sizeof(unsigned long));
  bsp_sync();

  for (unsigned long index = 0; index < sizes[core]; ++index)
    x[index] = 1;


  if (core == 0)
  {
    // Find the next prime under sqrt(N)
    for (; prime < sizes[0]; ++prime)
    {
      if (!x[prime])
        continue;

      // Send it to the others
      for (size_t proc = 1; proc < P; ++proc)
        bsp_put(proc, &prime, &prime, 0, sizeof(unsigned long));

      bsp_sync();

      // Sieve it on proc 0
      for (unsigned long multiple = prime * prime; multiple < sizes[0]; multiple += prime)
        x[multiple] = 0;
    }

    // Found all primes < N, so send a stopping signal
    prime = 0;

    for (size_t proc = 1; proc < P; ++proc)
      bsp_put(proc, &prime, &prime, 0, sizeof(unsigned long));

    bsp_sync();

  } else {
    while (1)
    {
      bsp_sync();

      // Break at prime = 0 signal
      if (prime == 0)
        break;

      // Otherwise, sieve the multiples
      for (unsigned long multiple = prime * prime; multiple < startsAt[core] + sizes[core]; multiple += prime)
        x[multiple - startsAt[core]] = 0;
    }
  }

  unsigned long count = 0;

  for (unsigned long index = 0; index < sizes[core]; ++index)
    if (x[index] == 1 && !(index < 2 && core==0))
      ++count;

  printf("Proc %zu found %zu primes\n", core, count);

  bsp_end();
}

int main(int argc, char ** argv)
{
  bsp_init(sieve, argc, argv);

  printf("How many processors do you want to use?\n");
  fflush(stdout);

  scanf("%zu", &P);

  if (P > bsp_nprocs())
  {
    printf("Sorry, not enough processors available.\n");
    fflush(stdout);
    exit(1);
  }

  if (P <= 1)
  {
    printf("Select at least two cores.\n");
    fflush(stdout);
    exit(1);
  }

  printf("Upper bound?\n");
  fflush(stdout);
  scanf("%lu", &N);

  sieve();

  printf("\n");
}
