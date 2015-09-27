#include "sieve.h"
#include "logPrimes.h"
#include "parallel.h"

#include <iostream>

extern unsigned long N;
extern MCBSP_PROCESSOR_INDEX_DATATYPE P;
extern unsigned long nPrint;

void logPrimes(bool isPrime[], unsigned long sizes[], unsigned long counters[], unsigned long startsAt[]) {
  size_t startingAtCore = 0;
  unsigned long total = 0;
  MCBSP_PROCESSOR_INDEX_DATATYPE core = bsp_pid();

  for (MCBSP_PROCESSOR_INDEX_DATATYPE proc = P - 1; proc >= 0; --proc)
  {
    total += counters[proc];
    if (total >= nPrint)
    {
      startingAtCore = proc;
      break;
    }
  }

  bsp_sync();

  for (int i = startingAtCore; i < P; i++)
  {
    if (core == i)
    {
      for (unsigned long j = 0; j < sizes[core]; j++)
      {
        if (isPrime[j] && !((core == 0) && (j < 2)))
        {
          --total;
          if(total <= nPrint)
            std::cout << startsAt[core] + j << '\n';
        }
      }
    }
    bsp_sync();
  }
}