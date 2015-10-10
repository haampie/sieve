#include "sieve_bucket.h"

#include <vector>
#include <cmath>

void sieve_bucket(
  std::vector<char> &smallerPrimes,
  std::vector<char> &isPrime,
  std::vector<size_t> &primes,
  std::vector<size_t> &indexInBucket,
  const size_t bucketSize,
  const size_t low,
  const size_t high,
  size_t &s,
  size_t &n
)
{
  std::fill(isPrime.begin(), isPrime.end(), 1);
  size_t next_pos;

  // store small primes needed to cross off multiples
  for (; s * s <= high; ++s)
  {
    if (smallerPrimes[s])
    {
      primes.push_back(s);
      next_pos = 0;
      if (s * s < low)
      {
        next_pos = (ceil(low / (float)s) - s);

        if (next_pos % 2 == 1)
          next_pos += 1;

        next_pos = s * next_pos;
      }
      indexInBucket.push_back( s * s + next_pos - low);
    }
  }

  for (size_t i = 1; i < primes.size(); ++i)
  {
    size_t j = indexInBucket[i];

    for (size_t k = 2 * primes[i] ; j < std::min(bucketSize, high - low); j += k)
      isPrime[j] = 0;

    indexInBucket[i] = j - bucketSize;
  }

  for (; n <= high; n += 2)
    if (isPrime[n - low])
      primes.push_back(n);
}