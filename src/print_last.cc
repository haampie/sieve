#include "print_last.h"

#include <iostream>
#include <vector>

void printLast(std::vector<size_t>* truePrimes, unsigned long int P, size_t * counters, int nPrint) {
  unsigned long start_core = 0;
  size_t skip_print = 0;
  for (int i = P - 1; i >= 0; i--) {
    if (nPrint <= counters[i]) {
      start_core = i;
      skip_print = counters[i] - nPrint;
      break;
    }
  }

  unsigned long int current_core = bsp_pid();
  for (unsigned long core = start_core; core < P; core ++) {
    bsp_sync();
    if (current_core == core) {
      for (size_t i = skip_print; i < (*truePrimes).size(); i++) {
        std::cout << (*truePrimes)[i] << "\n";
      }
    }
    skip_print = 0;
  }
  bsp_sync();
}