#include "sieve.h"
#include "logPrimes.h"
#include "parallel.h"
#include <iostream>
#include <math.h>

extern unsigned long N;
extern MCBSP_PROCESSOR_INDEX_DATATYPE P;
extern unsigned long nPrint;

unsigned long max(unsigned long a, unsigned long b) {
  return (a > b) ? a : b;
}

unsigned long min(unsigned long a, unsigned long b) {
  return (a < b) ? a : b;
}

void sieve() {

  bsp_begin(P);

  double time0, time1;
  time0 = bsp_time();

  MCBSP_PROCESSOR_INDEX_DATATYPE core = bsp_pid();

  // Find sizes for each proc and initialize arrays

  unsigned long sizes[P];
  unsigned long startsAt[P];

  sizes[0] = max((unsigned long) sqrt(N) + 1, (unsigned long) (N / P));
  startsAt[0] = 0;

  unsigned long size = (N - sizes[0] + 1) / (P - 1);
  MCBSP_PROCESSOR_INDEX_DATATYPE remains = (N - sizes[0] + 1) % (P - 1);

  for (MCBSP_PROCESSOR_INDEX_DATATYPE index = 1; index < P; ++index)
  {
    sizes[index] = size;

    if (index <= remains)
      ++sizes[index];

    startsAt[index] = startsAt[index - 1] + sizes[index - 1];
  }

  bool numbers[sizes[core]];
  unsigned long prime = 2;

  bsp_push_reg(&prime, sizeof(unsigned long));
  bsp_sync();

  for (unsigned long index = 0; index < sizes[core]; ++index)
    numbers[index] = true;

  // Start the sieve process

  unsigned long L = min(sizes[0], sqrt(N) + 1);
  while (prime < L)
  {
    if (core == 0)
    {
      // Find the next prime under sqrt(N)

      while (!numbers[prime])
        ++prime;

      // Send it to the others
      for (MCBSP_PROCESSOR_INDEX_DATATYPE proc = 1; proc < P; ++proc)
        bsp_put(proc, &prime, &prime, 0, sizeof(unsigned long));
    }

    bsp_sync();

    // Sieve all cores
    for (unsigned long multiple = prime * prime; multiple < startsAt[core] + sizes[core]; multiple += prime)
      numbers[multiple - startsAt[core]] = false;

    prime ++;
  }

  // Count the primes in each core and find the total

  unsigned long count = 0;

  for (unsigned long index = 0; index < sizes[core]; ++index)
    if (numbers[index] == true && !((core == 0) && (index < 2)))
      ++count;

  // Send the counters to all processors
  unsigned long counters[P];
  bsp_push_reg(counters, P * (sizeof(unsigned long)));
  bsp_sync();

  for (MCBSP_PROCESSOR_INDEX_DATATYPE i = 0; i < P; i++)
    bsp_put(i, &count, counters, core * sizeof(unsigned long), sizeof(unsigned long));

  bsp_sync();

  for (MCBSP_PROCESSOR_INDEX_DATATYPE i = (P - 1); i > 0; i--)
    counters[i - 1] += counters[i];

  if (core == 0)
  {
    std::cout << "There are " << counters[0] << " primes in under " << N << ". ";
    std::cout << "The last " << min(nPrint, counters[0]) << " are:\n";
  }


  // Print the last primes
  logPrimes(numbers, sizes, counters, startsAt);

  // Find the time taken
  time1 = bsp_time();

  if (core == 0)
    std::cout << "Time taken: " << time1 - time0 << '\n';

  bsp_end();
}