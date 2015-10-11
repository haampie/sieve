#include "print_last.h"

#include <iostream>
#include "parallel.h"

void printLast(std::vector<size_t>* truePrimes, processors P, size_t * counters, int nPrint) {
  processors start_core = 0;
  size_t skip_print = 0;
  for (int i = P - 1; i >= 0; i--) {
    if (nPrint <= counters[i]) {
      start_core = i;
      skip_print = counters[i] - nPrint;
      break;
    }
  }

  processors current_core = bsp_pid();
  for (processors core = start_core; core < P; core ++) {
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