#include "segmented_sieve.h"
#include "segment_boundaries.h"
#include "segment_boundaries_2.h"
#include "check_twin.h"
#include "first_cross.h"
#include "print_last.h"
#include "print_last_twins.h"
#include "goldbach.h"
#include "../config.h"

#include <iostream>
#include <vector>
#include <cmath>

const size_t CACHE_SIZE =  5 * 1024 * 7;
extern processors P;
extern size_t limit;
extern size_t nPrint;
extern size_t n_GBPrint;
extern N_PROGRAM program;

using namespace std;

void segmented_sieve()
{
  bsp_begin(P);
  processors core = bsp_pid(); // core id
  size_t root = sqrt(static_cast<double>(limit));
  size_t count = 0;
  size_t twinCount = 0;
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
    ++n;


  vector<char> bucket(bucketSize);

  double start = bsp_time();
  char latestInBucketWasPrime = 0;

  // Save the first and last prime of segment for twin program
  size_t firstPrimeNumberInSegment = 0;
  size_t lastPrimeNumberInSegment = 0;
  char firstPrimeInSegmentFound = 0;

  for (size_t bucketStart = segmentStart; bucketStart < segmentEnd; bucketStart += bucketSize)
  {
    // Upper bound of the current bucket
    size_t bucketEnd = min(bucketStart + bucketSize - 1, segmentEnd);

    // Assume all numbers in the bucket are prime
    fill(bucket.begin(), bucket.end(), 1);

    // The bucket can end before bucketStart + segmentStart
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

    switch (program)
    {
    case GENERATE:
      for (; n <= bucketEnd; n += 2)
      {
        if (bucket[n - bucketStart])
        {
          segmentPrimes.push_back(n);
          if (segmentPrimes.size() > nPrint) // stored more primes than we need to print
          {
            segmentPrimes.erase(segmentPrimes.begin());
          }
        }
      }
      break;
    case TWIN:
      // First prime at first index gets special attention
      // since it can be a twin across buckets
      if (bucket[n - bucketStart])
      {
        lastPrimeNumberInSegment = n;
        if (not firstPrimeInSegmentFound)
        {
          firstPrimeInSegmentFound = 1;
          firstPrimeNumberInSegment = n;
        }

        if (latestInBucketWasPrime)
        {
          ++twinCount;
          segmentPrimes.push_back(n);
        }
      }

      // Then go through the rest of the bucket
      for (n += 2; n <= bucketEnd; n += 2)
      {
        if (bucket[n - bucketStart])
        {
          // Save for intersegment twin primes

          lastPrimeNumberInSegment = n;
          if (not firstPrimeInSegmentFound)
          {
            firstPrimeInSegmentFound = 1;
            firstPrimeNumberInSegment = n;
          }

          // We found a twin.
          if (bucket[n - bucketStart - 2])
          {
            ++twinCount;
            segmentPrimes.push_back(n);
          }
        }
      }

      // n is already above the bound, so the latest is at - 2.
      latestInBucketWasPrime = bucket[n - bucketStart - 2];
      break;
    case GOLDBACH:
      for (; n <= bucketEnd; n += 2)
      {
        if (bucket[n - bucketStart])
        {
          ++count;
          segmentPrimes.push_back(n);
        }
      }
      break;
    }
  }
  double diff = bsp_time() - start;

  /********** Sieving done! Now the counters need to be added **********/

  size_t counters[P]; // will hold the counters of all cores

  switch (program)
  {
  case TWIN:
    // Push the first prime of current segment to the previous core
    size_t nextPrime;
    bsp_push_reg(&nextPrime, sizeof(size_t));
    bsp_sync();

    if (core > 0)
      bsp_put(core - 1, &(firstPrimeNumberInSegment), &nextPrime, 0, sizeof(size_t));

    bsp_sync();

    // Check for a twin prime on the boundary
    if (core < P - 1 and lastPrimeNumberInSegment + 2 == nextPrime)
    {
      segmentPrimes.push_back(nextPrime);
      ++twinCount;
    }

    count = segmentPrimes.size();
    
    bsp_push_reg(&counters, P * sizeof(size_t));
    bsp_sync();

    for (processors i = 0; i < P; i++)
      bsp_put(i, &count, &counters, core * sizeof(size_t), sizeof(size_t));
    bsp_sync();

    for (processors i = P - 1; i > 0; i--)
      counters[i - 1] += counters[i];

    printLastTwins(&segmentPrimes, P, counters, nPrint);

    break;
  case GENERATE:
    bsp_push_reg(&counters, P * sizeof(size_t));
    bsp_sync();

    count = segmentPrimes.size();

    for (processors i = 0; i < P; i++)
      bsp_put(i, &count, &counters, core * sizeof(size_t), sizeof(size_t));
    bsp_sync();

    for (processors i = P - 1; i > 0; i--)
      counters[i - 1] += counters[i];
    printLast(&segmentPrimes, P, counters, nPrint);

    break;
  case GOLDBACH:
    if (nPrint != 0) {
      if (core == 0)
        segmentPrimes.resize(counters[0], 0); // make the vector in core 0 larger

      bsp_push_reg(&(segmentPrimes[0]), segmentPrimes.size()*sizeof(size_t)); // register the vectors
      bsp_sync();

      for (int i = 1; i < P; i++) {
        if (core == i) // each core sends their vector to part of the vector in core 0
          bsp_put(0, &(segmentPrimes[0]), &(segmentPrimes[0]), (counters[0] - counters[i])*sizeof(size_t), count * sizeof(size_t));
        bsp_sync();
      }

      if (core == 0)
        goldbach(&segmentPrimes, limit, nPrint);
    }
    break;
  }

  bsp_sync();

  for (processors i = 0; i < P; ++i)
  {
    if (i == core)
      cout << "Core " << i << " took: " << diff << '\n';

    bsp_sync();
  }

  bsp_end();
}
