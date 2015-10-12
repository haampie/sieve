#include <vector>
#include <iostream>
#include <string>
#include <cmath>

void goldbach(std::vector<size_t>* truePrimes, size_t bound, size_t n_print) {

  int checkGB[(bound - 1) / 2 + 1] = {false};
  // Fill a vector of length (bound-1)/2 with false.   these bools represent all the even numbers
  // 2,4,6,..,bound. Then we will see if all   these may be written as a sum of two (odd) primes.
  // The number 2m corresponds to index m - 1 in checkGB. Similarly to the number n corresponds
  // the index n/2 - 1.

  size_t goldbach_pairs[n_print][2];

  size_t g_size = truePrimes->size(); // size for the goldbach part

  if ( n_print >= bound / 2 - 1) { // make sure we are not asking to print too many arguments
    printf("2 + 2 = 4\n");
    n_print = bound / 2 - 2;
  }

  // In this loop we check to see if all even numbers can we written as a sum of two primes. This
  // is done by looping through the primes and crossing off the

  size_t i, j;
  size_t sum;
  for (i = 0; i < g_size; i++)
    for (j = i; j < g_size; j++) {
      sum = (*truePrimes)[i] + (*truePrimes)[j];
      if ( sum <= bound) {
        if ( !(checkGB[ sum / 2 - 1 ]) )
          if ( sum > bound - 2 * n_print  )
          {
            goldbach_pairs[ n_print - 1 + ((*truePrimes)[i] + (*truePrimes)[j] - bound) / 2  ][0] = (*truePrimes)[i];
            goldbach_pairs[ n_print - 1 + ((*truePrimes)[i] + (*truePrimes)[j] - bound) / 2  ][1] = (*truePrimes)[j];
          }
        checkGB[ sum / 2 - 1 ] = true;
      } else {
        break;
      }
    }

  for (i = 0; i < n_print; i++)
    printf("%lu + %lu = %lu\n", goldbach_pairs[i][0], goldbach_pairs[i][1], goldbach_pairs[i][1] + goldbach_pairs[i][0]);

  for (int i = 2; i < (bound - 1) / 2 + 1; i++) // We don't check 2 and 4, so i starts at i.
    if (!checkGB[i]) {
      printf("The Golbach conjecture is false.\n");
      break;
    }
}
