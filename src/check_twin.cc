#include "check_twin.h"

#include <iostream>
#include <cmath>

void checkTwin(std::vector<size_t>* truePrimes, size_t extra_prime, processors P) {
  processors core = bsp_pid();
  for (processors i = 0; i < P; i++) {
    if (core == i) {
      int p_size = (*truePrimes).size();
      for (int i = 0; i < p_size - 1; i++)
        if ((*truePrimes)[i] + 2 == (*truePrimes)[i + 1])
          std::cout << (*truePrimes)[i] << " " << (*truePrimes)[i + 1] << "\n";
      if ((*truePrimes)[p_size - 1] + 2 == extra_prime)
        std::cout << extra_prime - 2 << " " << extra_prime << "\n";
    }
    bsp_sync();
  }
}