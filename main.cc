#include "main.h"
#include "config.h"
#include "src/segmented_sieve.h"
#include "src/parallel.h"

#include <iostream>
#include <string>

processors P = 4;
size_t limit = 1000000000;
size_t nPrint = 100;

using namespace std;

int main(int argc, char ** argv)
{
  if(argc > 1)
    limit = stoul(argv[1]);

  if(argc > 2)
    P = stoul(argv[2]);

  if(argc > 3)
    nPrint = stoul(argv[3]);

  bsp_init(segmented_sieve, argc, argv);
  segmented_sieve();
}
