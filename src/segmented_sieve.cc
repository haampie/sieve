#include "segmented_sieve.h"
#include "check_twin.h"
#include "first_cross.h"
#include "print_last.h"
#include "../config.h"

#include <iostream>
#include <vector>
#include <cmath>

const size_t CACHE_SIZE = 32 * 1024 * 7;
extern processors P;
extern size_t limit;
extern size_t nPrint;

using namespace std;

void segmented_sieve()
{
  bsp_begin(P);
  ++limit;
  size_t root = sqrt(static_cast<double>(limit));
  size_t count = 0;
  size_t twin_count = 0;

  // how big is the interval that we sieve over each time
  size_t bucketSize = CACHE_SIZE;

  // Find all the primes below root by sieving all primes below sqrt(sqrt(limit))

  vector<char> isSmallPrime(root + 1, 1);
  for (size_t i = 2; i * i <= root; ++i)
    if (isSmallPrime[i])
      for (size_t j = i * i; j <= root; j += i)
        isSmallPrime[j] = 0;

  /********** End of initial sieving **********/

  // This will hold the size of the vector contained on each core
  vector<size_t> sizes(P);

  // Starting values of the range of each core
  vector<size_t> startsAt(P);

  sizes[0] = max(root, limit / P);

  if (P > 1) {
    size_t approxSize = (limit - sizes[0]) / (P - 1);
    size_t rem = (limit - sizes[0]) % approxSize;

    for (processors core = 1; core < P; ++core)
      sizes[core] = approxSize;

    for (size_t core = 1; core < rem; ++core)
      ++sizes[core];

    startsAt[0] = 0;
    for (processors i = 1; i < P; ++i)
      startsAt[i] = startsAt[i - 1] + sizes[i - 1];
  }

  processors core = bsp_pid(); // core id

  // Will hold primes that need to be sieved for each bucket
  vector<size_t> bucketPrimes;

  // Bucketindex contains the local index where we first need to start sieving bucketPrimes 
  vector<size_t> bucketIndex;

  // Will contain the actual prime numbers
  vector<size_t> primes;

  size_t start = startsAt[core];
  size_t n = start - (start % 2) + 1;

  size_t endCore = startsAt[core] + sizes[core];
  size_t s = 2;

  int numberOfSievers = 0;
  vector<char> bucket(min(bucketSize, sizes[core]));

  for (size_t low = start; low < endCore; low += bucketSize)
  {
    // Upper bound of the current bucket
    size_t high = min(low + bucketSize - 1, endCore);

    // Assume all numbers in the bucket are prime
    fill(bucket.begin(), bucket.end(), 1);

    size_t currentBucketSize = high - low;

    // Search for new primes which should be used to sieve
    while (s * s <= high)
    {
      if (isSmallPrime[s])
      {
        ++numberOfSievers;
        primes.push_back(s);
        bucketIndex.push_back(firstCross(s, low));
      }
      ++s;
    }

    // Start sieving the bucket
    for (size_t primeIndex = 0; primeIndex < numberOfSievers; ++primeIndex)
    {
      size_t j = bucketIndex[primeIndex];
      
      // k denotes the step (skip even multiples)
      for (size_t k = 2 * primes[primeIndex]; j <= currentBucketSize; j += k )
        bucket[j] = 0;

      bucketIndex[primeIndex] = j - currentBucketSize - 1;
    }

    for (; n <= high; n += 2)
      if (bucket[n - low])
      {
        ++count;
        primes.push_back(n);
      }
  }

  /********** Sieving done! Now the counters need to be added **********/

  if (core == 0)
    ++primes[0];

  size_t extra_prime; // will hold the first prime of the next core for checking the twin primes
  size_t counters[P]; // will hold the counters of all cores
  bsp_push_reg(&extra_prime, sizeof(size_t)); // register them
  bsp_push_reg(&counters, P * sizeof(size_t));
  bsp_sync();

  // bsp_put((core - 1) % P, &(primes[0]), &extra_prime, 0, sizeof(size_t));
  // bsp_sync();

  // checkTwin(&primes, extra_prime, P);

  for (int i = 0; i < P; i++)
    bsp_put(i, &count, &counters, core * sizeof(size_t), sizeof(size_t));
  bsp_sync();
 
  for (int i = P - 1; i > 0; i--)
    counters[i - 1] += counters[i];

  printLast(&primes, P, counters, nPrint);

  if (core == 0)
    cout << counters[0] << " primes.\n";

  bsp_end();
}
