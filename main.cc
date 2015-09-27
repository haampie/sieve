#include "main.h"
#include "src/sieve.h"
#include "src/parallel.h"

#include <iostream>

unsigned long N = 100;
MCBSP_PROCESSOR_INDEX_DATATYPE P;
unsigned long nPrint;

int main(int argc, char ** argv)
{
  bsp_init(sieve, argc, argv);

  std::cout << "How many processors do you want to use?\n";
  std::cin >> P;

  if (P > bsp_nprocs())
  {
    std::cout << "Sorry, not enough processors available.\n";
    return 0;
  }

  if (P <= 1)
  {
    std::cout << "Select at least two cores.\n";
    return 0;
  }

  std::cout << "The last how many primes need to be printed?\n";
  std::cin >> nPrint;

  std::cout << "Upper bound?\n";
  std::cin >> N;

  std::cout << '\n';

  sieve();
}
