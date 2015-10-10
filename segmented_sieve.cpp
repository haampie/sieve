#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>
extern "C" {
#include <mcbsp.h>
}

const size_t CACHE_SIZE = 32 * 1024 * 7;
const size_t CACHE_SIZE = 32 * 1024;
int P = 5;

void segmented_sieve(size_t limit, size_t segment_size = CACHE_SIZE)
{
  size_t sqrt = (int) std::sqrt((double) limit);
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

  for (int core = 1; core < P; core++)
    sizes[core] = approxSize;

  for (int core = 1; core < rem; core++)
    sizes[core]++;

  startsAt[0] = 0;
  for (int i = 1; i < P; i++)
    startsAt[i] = startsAt[i - 1] + sizes[i - 1];

  unsigned long core = bsp_pid();

  std::vector<size_t> primes;
  std::vector<size_t> next;
  std::vector<size_t> truePrimes;

  size_t start = startsAt[core];
  size_t n = start - (start % 2) + 1;

  size_t next_pos;
  size_t end_core = startsAt[core] + sizes[core];
  size_t s = 2;

  std::vector<bool> sieve(std::min(segment_size, sizes[core]));

  for (size_t low = start; low < end_core; low += segment_size)
  {
    std::fill(sieve.begin(), sieve.end(), true);

    // current segment = interval [low, high]
    size_t high = std::min(low + segment_size - 1, end_core);

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
      for (size_t k = 2 * primes[i] ; j < std::min(segment_size, high - low); j += k) {
        sieve[j] = false;
      }
      next[i] = j - segment_size;
    }
    for (; n <= high; n += 2)
      if (sieve[n - low]) { // n is a prime
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
      std::cout << "\n";
      for (size_t i = 0; i < truePrimes.size(); i++) {
        std::cout << truePrimes[i] << "\n";
      }
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

}

int main(int argc, char** argv)
{
  size_t limit = 100000000;
  size_t size = CACHE_SIZE;
  bsp_begin(P);
  segmented_sieve(limit, size);
  bsp_end();

  return 0;
}
