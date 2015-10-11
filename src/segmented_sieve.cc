#include "segmented_sieve.h"
#include "../config.h"

#include <iostream>
#include <vector>
#include <cmath>

const size_t CACHE_SIZE = 32 * 1024 * 7;
extern processors P;
extern size_t limit;

void segmented_sieve()
{
  bsp_begin(P);
  limit += 1;
  size_t sqrt = (int) std::sqrt((double) limit);
  size_t count = 0;

  // Find all the primes below sqrt by sieving all primes below sqrt(sqrt(limit))

  std::vector<bool> is_prime(sqrt + 1, true);
  for (size_t i = 2; i * i <= sqrt; i++)
    if (is_prime[i])
      for (size_t j = i * i; j <= sqrt; j += i)
        is_prime[j] = false;

  /********** End of initial sieving **********/

  std::vector<size_t> sizes(P); // This will hold the size of the vector contained on each core
  std::vector<size_t> startsAt(P); // Starting values of the range of each core

  sizes[0] = std::max(sqrt, limit / P);

  if (P > 1) {
    size_t approxSize = (limit - sizes[0]) / (P - 1);
    size_t rem = (limit - sizes[0]) % approxSize;

    for (int core = 1; core < P; core++)
      sizes[core] = approxSize;

    for (int core = 1; core < rem; core++)
      sizes[core]++;

    startsAt[0] = 0;
    for (int i = 1; i < P; i++)
      startsAt[i] = startsAt[i - 1] + sizes[i - 1];
  }
  unsigned long core = bsp_pid(); // core id

  std::vector<size_t> primes; // Will hold primes that need to be sieved for each bucket
  std::vector<size_t> next; // Next[i] contains the local index where we first need to start sieving primes[i]
  std::vector<size_t> truePrimes; // Will contain the actual prime numbers

  size_t start = startsAt[core];
  size_t n = start - (start % 2) + 1;

  size_t end_core = startsAt[core] + sizes[core];
  size_t s = 2;

  std::vector<bool> local_sieve(std::min(size, sizes[core]));

  int bucket_size = CACHE_SIZE; // how big is the interval that we sieve over each time
  int num_primes = 0;

  for (size_t low = start; low < end_core; low += size)
  {

    size_t high = std::min(low + size - 1, end_core);
    std::fill(local_sieve.begin(), local_sieve.end(), true);

    bucket_size = high - low;

    while (s * s <= high)
    {
      if (is_prime[s])
      {
        num_primes++;
        primes.push_back(s);
        next.push_back(firstCross(s, low));
      }
      s++;
    }

    for (int t = 0; t < num_primes; t++)
    {
      int j = next[t];
      
      for (int k = 2 * primes[t]; j <= bucket_size; j += k )
        local_sieve[j] = false;

      next[t] = j - bucket_size - 1;
    }

    for (; n <= high; n += 2)
      if (local_sieve[n - low]) {
        count++;
        truePrimes.push_back(n);
      }
  }

  /********** Sieving done! Now the counters need to be added **********/

  if (core == 0)
    truePrimes[0]++;

  size_t extra_prime; // will hold the first prime of the next core for checking the twin primes
  size_t counters[P]; // will hold the counters of all cores
  bsp_push_reg(&extra_prime, sizeof(size_t)); // register them
  bsp_push_reg(&counters, P * sizeof(size_t));
  bsp_sync();

  bsp_put((core - 1) % P, &(truePrimes[0]), &extra_prime, 0, sizeof(size_t));
  bsp_sync();

  // checkTwin(&truePrimes, extra_prime, P);

  for (int i = 0; i < P; i++)
    bsp_put(i, &count, &counters, core * sizeof(size_t), sizeof(size_t));
  bsp_sync();

  for (int i = P - 1; i > 0; i--)
    counters[i - 1] += counters[i];

  printLast(&truePrimes, P, counters, nPrint);

  if (core == 0)
    std::cout << counters[0] << " primes.\n";

  bsp_end();
}