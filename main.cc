#include "main.h"
#include "config.h"
#include "src/segmented_sieve.h"
#include "src/parallel.h"

#include <iostream>
#include <string>
#include <cmath>

processors P = 6;
size_t limit = 100;
size_t nPrint = 4;
size_t n_GBPrint = 0;
N_PROGRAM program = TWIN;

using namespace std;

int main(int argc, char ** argv)
{
  if (argc > 1)
    limit = stoul(argv[1]);

  if (argc > 2)
    P = stoul(argv[2]);

  if (argc > 3)
    nPrint = stoul(argv[3]);

  // Find a suitable number of cores
  size_t segmentSize = limit / P;

  // If sqrt(N) > limit / P, then use one core
  if (limit > segmentSize * segmentSize)
  {
    P = 1;
  }

  bsp_init(segmented_sieve, argc, argv);
  segmented_sieve();
}
