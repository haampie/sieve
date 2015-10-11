#include "segmented_sieve.h"
#include "segment_boundaries.h"
#include "segment_boundaries_2.h"
#include "check_twin.h"
#include "first_cross.h"
#include "print_last.h"
#include "goldbach.h"
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
  size_t twin_count = 0;
  size_t sieverIndex = 0;

  // how big is the interval that we sieve over each time
  size_t bucketSize = CACHE_SIZE;

  vector<char> isSmallPrime(root + 1, 1);

  // Bucketindex contains the local index where we first need to start sieving bucketPrimes
  vector<size_t> bucketIndex;

  // Will contain the siever prime numbers
  vector<size_t> sievers;

  // Will contain primes of the whole segment
  vector<size_t> segmentPrimes;

  /********** End of initial sieving **********/

  size_t segmentStart;
  size_t segmentEnd;
  size_t startSegmentingFrom = 3;

  // segmentBoundariesTryTwo(&segmentStart, &segmentEnd, startSegmentingFrom, limit, P, core);
  segmentBoundaries(&segmentStart, &segmentEnd, startSegmentingFrom, limit, P, core);

  // For debuggin the boundaries per core, enable the code here:
  if (core == 0)
    cout << "From " << startSegmentingFrom << " to " << limit << '\n';

  for (processors i = 0; i < P; ++i)
  {
    if (core == i) {
      cout << segmentStart << ' ' << segmentEnd << "\n";
    }
    bsp_sync();
  }

  // Find all the primes below root by sieving all primes below sqrt(sqrt(limit))
  for (size_t i = 2; i * i <= root; ++i)
    if (isSmallPrime[i])
      for (size_t j = i * i; j <= root; j += i)
        isSmallPrime[j] = 0;

  // Add them to the siever list and mark their first occuren
  for (size_t i = 3; i <= root; ++i)
    if (isSmallPrime[i])
      sievers.push_back(i);

  size_t n = segmentStart;

  if (n % 2 == 0)
    --n;
  

  vector<char> bucket(bucketSize);

  for (size_t bucketStart = segmentStart; bucketStart < segmentEnd; bucketStart += bucketSize)
  {
    // Upper bound of the current bucket
    size_t bucketEnd = min(bucketStart + bucketSize - 1, segmentEnd);

    // Assume all numbers in the bucket are prime
    fill(bucket.begin(), bucket.end(), 1);

    // The bucket kan end before bucketStart + segmentStart
    size_t currentBucketSize = bucketEnd - bucketStart + 1;

    // Search for new primes which should be used to sieve
    while (sieverIndex < sievers.size() && sievers[sieverIndex] * sievers[sieverIndex] <= bucketEnd)
    {
      bucketIndex.push_back(firstCross(sievers[sieverIndex], bucketStart));
      ++sieverIndex;
    }

    // Start sieving the bucket
    for (size_t primeIndex = 0; primeIndex < sieverIndex; ++primeIndex)
    {
      size_t j = bucketIndex[primeIndex];

      // k denotes the step (skip even multiples)
      for (size_t k = 2 * sievers[primeIndex]; j < currentBucketSize; j += k )
        bucket[j] = 0;

      bucketIndex[primeIndex] = j - bucketSize;
    }

    for (; n <= bucketEnd; n += 2)
    {
      if (bucket[n - bucketStart])
      {
        ++count;
        // segmentPrimes.push_back(n);
      }
    }
  }

  /********** Sieving done! Now the counters need to be added **********/

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

  // printLast(&segmentPrimes, P, counters, nPrint);

  if (core == 0)
    cout << counters[0] << " primes.\n";

  bsp_end();
}
