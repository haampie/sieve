#include "sieve.h"
#include "logPrimes.h"
#include "parallel.h"

#include <iostream>

extern ullong N;
extern MCBSP_PROCESSOR_INDEX_DATATYPE P;
extern ullong nPrint;

void logPrimes(bool isPrime[], ullong sizes[], ullong counters[], ullong startsAt[]) {
  size_t startingAtCore = 0;
  ullong total = 0;
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

  for (MCBSP_PROCESSOR_INDEX_DATATYPE proc = startingAtCore; proc < P; ++proc)
  {
    if (core == proc)
    {
      for (ullong j = 0; j < sizes[core]; j++)
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