#include "check_twin.h"

#include <iostream>
#include <cmath>

void checkTwin(std::vector<size_t>* truePrimes, size_t extra_prime, processors P) {

  // unsigned long twin_print = 40;
  processors core = bsp_pid();

  // size_t twin_count = 0;
  // for (size_t i = 1; i < truePrimes->size(); i++){
  //   if ((*truePrimes)[i-1] == (*truePrimes)[i] - 2)
  //     twin_count ++;
  // }

  // size_t twin_counters(P);
  // bsp_push_reg(&twin_counters, P*sizeof(size_t));
    
  // for (int i = 0; i < P; i++)
  //   bsp_put(i, &twin_count, &twin_counters, core * sizeof(size_t), sizeof(size_t));
  // bsp_sync();

  // for (long unsigned int i = P - 1; i > 0; i--)
  //   twin_counters[i-1] += twin_counters[i];  
  
  for (processors i = 0; i < P; i++) 
  {
    if (core == i) 
    {
      size_t p_size = (*truePrimes).size();
      for (size_t i = 0; i < p_size - 1; i++)
        if ((*truePrimes)[i] + 2 == (*truePrimes)[i + 1])
          std::cout << (*truePrimes)[i] << " " << (*truePrimes)[i + 1] << "\n";

      if ((*truePrimes)[p_size - 1] + 2 == extra_prime)
        std::cout << extra_prime - 2 << " " << extra_prime << "\n";
    }
    bsp_sync();
  }
}
