#include "sieve.h"
#include "logPrimes.h"
#include "parallel.h"

#include <iostream>

extern unsigned long N;
extern MCBSP_PROCESSOR_INDEX_DATATYPE P;
extern unsigned long nPrint;

void logPrimes(bool numbers[], unsigned long sizes[], unsigned long counters[], unsigned long startsAt[]) {
  int printSkip[P];
  MCBSP_PROCESSOR_INDEX_DATATYPE core = bsp_pid();
  // Core where the first prime to be printed is
  int printCore;

  for (MCBSP_PROCESSOR_INDEX_DATATYPE i = 0; i < P; i++)
    printSkip[i] = 0;

  if (nPrint >= counters[0])
  {
    printCore = 0;
  }
  else
  {
    for (MCBSP_PROCESSOR_INDEX_DATATYPE i = (P - 1); i >= 0; i--)
    {
      if (counters[i] >= nPrint)
      {
        // How many primes in printCore we need to skip
        printSkip[i] = counters[i] - nPrint;
        printCore = i;
        break;
      }
    }
  }

  bsp_sync();

  for (int i = printCore; i < P; i++)
  {
    if (core == i)
    {
      for (unsigned long j = 0; j < sizes[core]; j++)
      {
        if (numbers[j] && !((core == 0) && (j < 2)))
        {
          if (printSkip[core] == 0)
            std::cout << startsAt[core] + j << '\n';
          else
            --printSkip[core];
        }
      }
    }
    bsp_sync();
  }
}