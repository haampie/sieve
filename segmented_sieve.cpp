#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>

const size_t CACHE_SIZE = 32768;

void segmented_sieve(size_t limit, size_t segment_size = CACHE_SIZE)
{
  size_t sqrt = (int) std::sqrt((double) limit);
  size_t count = (limit < 2) ? 0 : 1;
  size_t s = 2;
  size_t n = 3;

  // vector used for sieving
  std::vector<bool> sieve(segment_size);

  // generate small primes <= sqrt
  std::vector<bool> is_prime(sqrt + 1, true);
  for (size_t i = 2; i * i <= sqrt; i++)
    if (is_prime[i])
      for (size_t j = i * i; j <= sqrt; j += i)
        is_prime[j] = false;

  std::vector<size_t> primes;
  std::vector<size_t> next;

  for (size_t low = 0; low <= limit; low += segment_size)
  {
    std::fill(sieve.begin(), sieve.end(), true);

    // current segment = interval [low, high]
    size_t high = std::min(low + segment_size - 1, limit);

    // store small primes needed to cross off multiples
    for (; s * s <= high; s++)
    {
      if (is_prime[s])
      {
        primes.push_back(s);
        next.push_back(s * s - low);
      }
    }
    // sieve the current segment
    for (size_t i = 1; i < primes.size(); i++)
    {
      size_t j = next[i];
      for (size_t k = primes[i] * 2; j < segment_size; j += k)
        sieve[j] = false;
      next[i] = j - segment_size;
    }

    for (; n <= high; n += 2)
      if (sieve[n - low]) // n is a prime
        count++;
  }

  std::cout << count << " primes found." << std::endl;
}

int main(int argc, char** argv)
{
  size_t limit = 10000000000;
  size_t size = CACHE_SIZE;

  segmented_sieve(limit, size);
  return 0;
}
