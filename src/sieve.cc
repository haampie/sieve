#include "sieve.h"
#include "logPrimes.h"
#include "parallel.h"
#include <iostream>
#include <math.h>

extern ullong N;
extern MCBSP_PROCESSOR_INDEX_DATATYPE P;
extern ullong nPrint;

ullong max(ullong a, ullong b) {
  return (a > b) ? a : b;
}

ullong min(ullong a, ullong b) {
  return (a < b) ? a : b;
}

void sieve() {

  bsp_begin(P);

  double time0, time1;
  time0 = bsp_time();

  MCBSP_PROCESSOR_INDEX_DATATYPE core = bsp_pid();

  // Find sizes for each proc and initialize arrays

  ullong sizes[P];
  ullong startsAt[P];

  sizes[0] = max((ullong) sqrt(N) + 1, (ullong) (N / P));
  startsAt[0] = 0;

  if(P > 1)
  {
    ullong size = (N - sizes[0] + 1) / (P - 1);
    MCBSP_PROCESSOR_INDEX_DATATYPE remains = (N - sizes[0] + 1) % (P - 1);

    for (MCBSP_PROCESSOR_INDEX_DATATYPE index = 1; index < P; ++index)
    {
      sizes[index] = size;
      
      if (index <= remains)
        ++sizes[index];

      startsAt[index] = startsAt[index - 1] + sizes[index - 1];
    }
  }

  bool isPrime[sizes[core]];
  ullong prime = 2;

  bsp_push_reg(&prime, sizeof(ullong));
  bsp_sync();

  for (ullong index = 0; index < sizes[core]; ++index)
    isPrime[index] = true;

  // Start the sieve process
  ullong crossCtr = 0;
  
  ullong L = min(sizes[0], sqrt(N) + 1);
  while (prime < L)
  {
    if (core == 0)
    {
      // Find the next prime under sqrt(N)
      while (!isPrime[prime])
        ++prime;

      // Send it to the others
      for (MCBSP_PROCESSOR_INDEX_DATATYPE proc = 1; proc < P; ++proc)
        bsp_put(proc, &prime, &prime, 0, sizeof(ullong));
    }

    bsp_sync();

    // Sieve all cores
    for (ullong multiple = prime * prime; multiple < startsAt[core] + sizes[core]; multiple += prime)
      {
	isPrime[multiple - startsAt[core]] = false;
	crossCtr++;
      }
	
    ++prime;
  }

  // Count the primes in each core and find the total

  ullong locally = 0;
  ullong total = 0;
  ullong counters[P];

  for (ullong index = 0; index < sizes[core]; ++index)
    if (isPrime[index] == true && !((core == 0) && (index < 2)))
      ++locally;

  // Send the counters to all processors
  bsp_push_reg(counters, P * (sizeof(ullong)));
  bsp_sync();

  for (MCBSP_PROCESSOR_INDEX_DATATYPE proc = 0; proc < P; ++proc)
    bsp_put(proc, &locally, counters, core * sizeof(ullong), sizeof(ullong));

  bsp_sync();

  for (MCBSP_PROCESSOR_INDEX_DATATYPE proc = 0; proc < P; ++proc)
    total += counters[proc];

  if (core == 0)
  {
    std::cout << "There are " << total << " primes in under " << N << ". ";
    std::cout << "The last " << min(nPrint, total) << " are:\n";
  }

  // Print the last primes
  logPrimes(isPrime, sizes, counters, startsAt);

  // Find the time taken
  time1 = bsp_time();

  if (core == 0)
    std::cout << "Time taken: " << time1 - time0 << '\n';

  bsp_sync();

  for (MCBSP_PROCESSOR_INDEX_DATATYPE proc = 0; proc < P; ++proc)
    {
      if (core == proc)
	std::cout << "Processor " << core << " crossed out " << crossCtr << " numbers.\n";
      bsp_sync();
    }

  
  bsp_end();
}
