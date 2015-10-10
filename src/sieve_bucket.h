#ifndef __INCLUDE_SIEVEBUCKET__
#define __INCLUDE_SIEVEBUCKET__

#include <vector>
#include <iostream>

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
);

#endif