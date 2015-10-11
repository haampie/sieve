#include "segmented_sieve.h"
#include "segment_boundaries.h"
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
  processors core = bsp_pid(); // core id
  size_t root = sqrt(static_cast<double>(limit));
  size_t count = 0;

  // how big is the interval that we sieve over each time
  size_t bucketSize = CACHE_SIZE;

  // Find all the primes below root by sieving all primes below sqrt(sqrt(limit))

  vector<char> isSmallPrime(root + 1, 1);
  for (size_t i = 2; i * i <= root; ++i)
    if (isSmallPrime[i])
      for (size_t j = i * i; j <= root; j += i)
        isSmallPrime[j] = 0;

  /********** End of initial sieving **********/

  size_t segmentStart;
  size_t segmentEnd;

  segmentBoundaries(&segmentStart, &segmentEnd, 3, limit, P, core);

  // For debuggin the boundaries per core, enable the code here:
  // if(core == 0)
  //   cout << "From " << 3 << " to " << limit << '\n';
  // bsp_sync();
  // for(processors i = 0; i < P; ++i)
  // {
  //   if(core == i) {
  //     cout << segmentStart << ' ' << segmentEnd << "\n";
  //   }
  //   bsp_sync();
  // }

  // Bucketindex contains the local index where we first need to start sieving bucketPrimes 
  vector<size_t> bucketIndex;

  // Will contain the actual prime numbers
  vector<size_t> primes;

  size_t s = 2;
  size_t n = segmentStart;

  vector<char> bucket(bucketSize);

  for (size_t bucketStart = segmentStart; bucketStart < segmentEnd; bucketStart += bucketSize)
  {
    // Upper bound of the current bucket
    size_t bucketEnd = min(bucketStart + segmentStart, segmentEnd) - 1;

    // Assume all numbers in the bucket are prime
    fill(bucket.begin(), bucket.end(), 1);

    // The bucket kan end before bucketStart + segmentStart
    size_t currentBucketSize = bucketEnd - bucketStart + 1;

    // Search for new primes which should be used to sieve
    while (s * s <= bucketEnd)
    {
      if (isSmallPrime[s])
      {
        primes.push_back(s);
        bucketIndex.push_back(firstCross(s, bucketStart));
      }
      ++s;
    }

    // Start sieving the bucket
    for (size_t primeIndex = 0; primeIndex < primes.size(); ++primeIndex)
    {
      size_t j = bucketIndex[primeIndex];
      
      // k denotes the step (skip even multiples)
      for (size_t k = 2 * primes[primeIndex]; j < currentBucketSize; j += k )
        bucket[j] = 0;

      bucketIndex[primeIndex] = j - bucketSize;
    }

    for (; n <= bucketEnd; n += 2)
    {
      if (bucket[n - bucketStart])
      {
        ++count;
        primes.push_back(n);

        if(core == 0)
        {
          cout << n << '\n';
        }
      }
    }


    for(processors i = 0; i < P ; ++i)
    {
      if(core == i)
      {
        cout << "Core " << i << '\n';
        for(size_t i = 0; i < primes.size(); ++i)
        {
          cout << primes[i] << ' ';
        }
        cout << '\n';
      }
      bsp_sync();
    }
    return;
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