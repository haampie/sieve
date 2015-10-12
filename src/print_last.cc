#include "print_last.h"

#include <iostream>
#include "parallel.h"

using namespace std;

void printLast(vector<size_t> *truePrimes, processors P, size_t *counters, int nPrint)
{
  // Determine in what core we need to starr printing and what the local index is of 
  // the first prime that needs to be printed.

  processors startCore = 0;
  size_t skipPrint = 0;
  for (int i = P - 1; i >= 0; --i)
  {
    if (nPrint <= counters[i]) // There are more that nPrint primes contained in cores i,..,P-1.
    {
      startCore = i;
      skipPrint = counters[i] - nPrint; // How many primes in core i may be skipped
      break;
    }
  }

  processors current_core = bsp_pid();
  for (processors core = startCore; core < P; ++core)
  {
    bsp_sync();

    if (current_core == core) // make sure the cores only print one at a time
      for (size_t i = skipPrint; i < (*truePrimes).size(); ++i) 
        cout << (*truePrimes)[i] << '\n';

    skipPrint = 0;
  }
  bsp_sync();
}