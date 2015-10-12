#include "main.h"
#include "config.h"
#include "src/segmented_sieve.h"
#include "src/parallel.h"

#include <cstdio>
#include <iostream>
#include <string>
#include <cmath>

processors P = 6;
size_t limit = 100;
size_t nPrint = 4;
N_PROGRAM program;

using namespace std;

int main(int argc, char ** argv)
{
  if(argc != 5)
  {
    cout << "Incorrect input\n";
    return 0;
  }

  size_t procs;

  sscanf(argv[2], "%lu", &procs);
  sscanf(argv[3], "%lu", &limit);
  sscanf(argv[4], "%lu", &nPrint);

  P = static_cast<processors>(procs);

  string programType = argv[1];

  if(programType == "--generate")
  {
    program = GENERATE;
  }
  else if(programType == "--twin")
  {
    program = TWIN;
    limit += 2;
  }
  else if(programType == "--goldbach")
  {
    program = GOLDBACH;
  }
  else
  {
    cout << "Wrong program type\n";
    return 0;
  }

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
