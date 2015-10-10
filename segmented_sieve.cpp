#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>
extern "C" {
#include <mcbsp.h>
}

const size_t CACHE_SIZE = 8000;
size_t limit = 1000000000;
size_t size = CACHE_SIZE;
int P = 5;
int print = 0; // print de gevonden priem getallen en wat data

// For an array starting at low, this function returns the local index of the first multiple of s that needs to be crossed out.
int firstCross(int s, int low){
  if (s*s > low)
    return (s*s - low);
  
  if ((low - s*s) % (2*s) == 0)
    return 0;
  
  return (2*s - ((low - s*s) % (2*s)));
}

void segmented_sieve(size_t limit, size_t segment_size = CACHE_SIZE)
{
  size_t sqrt = (int) std::sqrt((double) limit);
  size_t count = 0;

  // Find all the primes below sqrt by sieving all primes below sqrt(sqrt(limit)) 

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
    startsAt[i] = startsAt[i-1] + sizes[i-1];
  
  unsigned long core = bsp_pid();
  
  std::vector<size_t> primes;
  std::vector<size_t> next;
  std::vector<size_t> truePrimes;
  
  size_t start = startsAt[core];
  size_t n = start - (start % 2) + 1; 

  size_t end_core = startsAt[core] + sizes[core];
  size_t s = 2;

  std::vector<bool> sieve(std::min(size, sizes[core]));
  int bucket_size;
  int next_pos;
  int num_primes = 0;
  
  for (size_t low = start; low < end_core; low += size)
    {
      std::fill(sieve.begin(), sieve.end(), true);
      // current segment = interval [low, high]
      size_t high = std::min(low + size - 1, end_core);
      bucket_size = high - low;
     
      while (s * s <= high){
	if (is_prime[s]){
	  num_primes++;
	  primes.push_back(s);
	  next.push_back(firstCross(s,low));
	}
	s++;
      }

      for (int t = 0; t < num_primes; t++){
	int j = next[t];      
	for (int k = 2*primes[t]; j <= high-low; j += k ){
	  sieve[j] = false;
	}
	next[t] = firstCross(primes[t], high + 1);
      }
            
      for (; n <= high; n += 2)
	if (sieve[n - low]){
	  count++;
       	  truePrimes.push_back(n);
       	}
    }

  if (core == 0)
    truePrimes[0]++;

  if (print == 1)  
    for (int i = 0; i < P; i++){
      bsp_sync();
      if (bsp_pid() == i){
	std::cout << "\n";
	for (size_t i = 0; i < size; i++){
	  std::cout << truePrimes[i] << "\n";
	}
	std::cout << count << " primes found." << "\n";
	bsp_sync();
      }
    }


  
  /********** Sieving done! Now the counters need to be added **********/
  
  size_t counters[P];
  bsp_push_reg(&counters, P*sizeof(size_t));
  bsp_sync();

  for (int i = 0; i < P; i++)
    bsp_put(i,&count,&counters,core*sizeof(size_t),sizeof(size_t));
  
  bsp_sync();

  for (int i = P-1; i > 0; i--)
    counters[i-1] += counters[i];

  if (core == 0)
      std::cout << counters[0] << " primes.\n"; 
  
}

int main(int argc, char** argv)
{
  bsp_begin(P);
  segmented_sieve(limit, size);
  bsp_end();  
  std::cout << firstCross(3,33);
  
  return 0;
}
