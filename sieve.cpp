#include <iostream>
#include <math.h>
#include <vector>
#include <string>
#include <algorithm>
#include <mcbsp.h>

using namespace std;

/**
 *  Interval:             [100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110]
 *  Zonder even getallen: [101, 103, 105, 107, 109]
 *  Geeft bucket index:   [0,     1,   2,   3,   4]
 */

int P = 6;
size_t limit = 1000000;
size_t halfLimit = limit / 2;
size_t sqrtPrimes = sqrt(limit) / 2;
size_t fourthRoot = static_cast<size_t>(sqrt(sqrt(static_cast<double>(limit)))) / 2;
size_t bucketSize = 32 * 1024 * 7;
// size_t bucketSize = 100;
size_t counter = 0;
size_t latestUsedSieverIndex = 0;


void spmd() {
  return;
}


int main(int argc, char *argv[])
{
  size_t limit = 100000000;

  if (argc == 2)
    limit = stoul(argv[1]);

  size_t halfLimit = limit / 2;
  size_t sqrtPrimes = sqrt(limit) / 2;
  size_t fourthRoot = static_cast<size_t>(sqrt(sqrt(static_cast<double>(limit)))) / 2;
  size_t bucketSize = 32 * 1024 * 7;
  // size_t bucketSize = 100;
  size_t counter = 0;
  size_t latestUsedSieverIndex = 0;

  // For finding the first primes.
  vector<bool> smallestPrimes(sqrtPrimes + 1, true);

  // Shows whether a certain number in a bucket is prime.
  vector<bool> bucket(bucketSize);

  // Keeps track of all primes below sqrt(N)
  vector<size_t> sievers;
  vector<size_t> applicableSievers;

  // Keeps track of local bucket index of the first odd multiple of each siever prime number
  vector<size_t> start_at;

  // First find the primes below sqrt(N)
  // This requires finding the primes up to sqrt(sqrt(N)) and sieving up to sqrt(N)
  for (size_t i = 1; i <= fourthRoot; ++i)
  {
    if (smallestPrimes[i])
    {
      size_t prime = 2 * i + 1;
      sievers.push_back(prime);
      for (size_t j = 2 * (i * i + i); j <= sqrtPrimes; j += prime)
        smallestPrimes[j] = false;
    }
  }

  // Append all remaining siever primes to the list
  for (size_t i = fourthRoot + 1; i <= sqrtPrimes; ++i)
    if (smallestPrimes[i])
    {
      size_t prime = 2 * i + 1;
      sievers.push_back(prime);
    }

  size_t numberOfSievers = sievers.size();
  start_at.resize(numberOfSievers);


  /**
   * Now start sieving from 0 to N in intervals of size bucketSize.
   */
  for (size_t low = 0; low <= halfLimit; low += bucketSize)
  {

    size_t high = min(halfLimit - low, bucketSize);

    // Set them all to primes.
    fill(bucket.begin(), bucket.end(), true);

    size_t bucketMax = 2 * (low + high);

    while (latestUsedSieverIndex < numberOfSievers)
    {
      size_t prime = sievers[latestUsedSieverIndex];
      size_t square = prime * prime;

      if (square >= bucketMax) break;

      start_at[latestUsedSieverIndex] = (square - low * 2 - 1) / 2;
      ++latestUsedSieverIndex;
    }


    // Sieve the current bucket.
    for (size_t i = 0; i < latestUsedSieverIndex; ++i)
    {
      // j should be scoped here, so that we can calculate the index for the next bucket.
      size_t j = start_at[i];

      for (size_t k = sievers[i]; j < high; j += k)
        bucket[j] = false;

      start_at[i] = j - bucketSize;
    }

    for (size_t i = 0; i < high; ++i)
      if (bucket[i])
        ++counter;
  }

  cout << counter << " primes <= " << limit << '\n';

  return 0;
}
