#ifndef FUNCTIONS
#define FUNCTIONS

int firstCross(int s, int low) {
  if (s * s > low)
    return (s * s - low);
  if ((low - s * s) % (2 * s) == 0)
    return 0;
  return (2 * s - ((low - s * s) % (2 * s)));
}

void checkTwin(std::vector<size_t>* truePrimes, size_t extra_prime, unsigned long int P) {
  unsigned long core = bsp_pid();
  for (unsigned long i = 0; i < P; i++) {
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



#endif
