#include "print_last.h"

#include <iostream>
#include "parallel.h"

void printLast(std::vector<size_t>* truePrimes, processors P, size_t * counters, int nPrint)
{
  processors startCore = 0;
  size_t skipPrint = 0;
  for (int i = P - 1; i >= 0; i--)
  {
    if (nPrint <= counters[i])
    {
      startCore = i;
      skipPrint = counters[i] - nPrint;
      break;
    }
  }

  processors current_core = bsp_pid();
  for (processors core = startCore; core < P; core ++)
  {
    bsp_sync();

    if (current_core == core)
      for (size_t i = skipPrint; i < (*truePrimes).size(); i++)
        std::cout << (*truePrimes)[i] << "\n";
    
    skipPrint = 0;
  }
  bsp_sync();
}