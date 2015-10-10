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
  size_t sqrt = (int) std::sqrt((double) limit);
  size_t bucketSize = CACHE_SIZE;
  size_t count = 0;

  // generate small primes <= sqrt
  std::vector<bool> is_prime(sqrt + 1, true);
  for (size_t i = 2; i * i <= sqrt; i++)
    if (is_prime[i])
      for (size_t j = i * i; j <= sqrt; j += i)
        is_prime[j] = false;

  /********** End of initial sieving **********/

  std::vector<size_t> sizes(P);
  std::vector<size_t> startsAt(P);
  sizes[0] = std::max(sqrt, limit / P);

  size_t approxSize = (limit - sizes[0]) / (P - 1);
  size_t rem = (limit - sizes[0]) % approxSize;

  for (processors core = 1; core < P; ++core)
    sizes[core] = approxSize;

  for (size_t core = 1; core < rem; ++core)
    sizes[core]++;

  startsAt[0] = 0;
  for (processors i = 1; i < P; i++)
    startsAt[i] = startsAt[i - 1] + sizes[i - 1];

  size_t core = bsp_pid();

  std::vector<size_t> primes;
  std::vector<size_t> next;
  std::vector<size_t> truePrimes;

  size_t start = startsAt[core];
  size_t n = start - (start % 2) + 1;

  size_t next_pos;
  size_t end_core = startsAt[core] + sizes[core];
  size_t s = 2;

  /** 
   * isPrime tells whether the ith element in the bucket is prime
   * We use char since it is only one byte long
   */
  std::vector<char> isPrime(std::min(bucketSize, sizes[core]));

  // Start sieving bucket by bucket
  // TODO: move this to a seperate function.
  for (size_t low = start; low < end_core; low += bucketSize)
  {
    std::fill(isPrime.begin(), isPrime.end(), 1);

    // current segment = interval [low, high]
    size_t high = std::min(low + bucketSize - 1, end_core);

    // store small primes needed to cross off multiples
    for (; s * s <= high; s++) {
      if (is_prime[s]) {
        primes.push_back(s);
        next_pos = 0;
        if (s * s < low) {
          next_pos = (ceil(low / (float)s) - s);
          if (next_pos % 2 == 1)
            next_pos += 1;
          next_pos = s * next_pos;
        }
        next.push_back( s * s + next_pos - low);
      }
    }

    for (size_t i = 1; i < primes.size(); i++)
    {
      size_t j = next[i];

      for (size_t k = 2 * primes[i] ; j < std::min(bucketSize, high - low); j += k)
        isPrime[j] = 0;

      next[i] = j - bucketSize;
    }
    for (; n <= high; n += 2)
      if (isPrime[n - low]) { // n is a prime
        count++;
        truePrimes.push_back(n);
      }
  }

  if (core == 0)
    truePrimes[0]++;

  /********** A sanity check print to screen *************/

  for (int i = 0; i < P; i++) {
    bsp_sync();
    if (bsp_pid() == i) {
      // std::cout << "\n";
      // for (size_t i = 0; i < truePrimes.size(); i++) {
      //   std::cout << truePrimes[i] << "\n";
      // }
      std::cout << count << " primes found." << "\n";
      bsp_sync();
    }
  }

  size_t counters[P];
  bsp_push_reg(&counters, P * sizeof(size_t));
  bsp_sync();

  for (int i = 0; i < P; i++)
    bsp_put(i, &count, &counters, i * sizeof(size_t), sizeof(size_t));

  bsp_sync();

  if (core == 1)
    for (int i = 0; i < P ; i++)
      //std::cout << counters[i] << "\t";

      for (int i = P - 1; i > 0; i--)
        counters[i - 1] += counters[i];

  //  std::cout << counters[0] << "\n";
  bsp_end();
}