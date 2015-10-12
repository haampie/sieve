#include <iostream>
#include <math.h>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char const *argv[])
{
  size_t bound = 100;
  size_t counter = 0;
  size_t operations = 0;

  if (argc == 2)
    bound = stoul(argv[1]);

  vector<bool> numbers(bound, true);

  size_t root = floor(sqrt(bound));

  for (size_t i = 3; i <= root; i += 2)
  {
    operations += 2; // For adding 2 to i and i <= root
    if (numbers[i])
    {
      ++operations; // For i*i
      for (size_t j = i * i; j < bound; j += i)
      {
        operations += 2; // For += i and j < bound
        numbers[j] = false;
      }
    }
  }

  for (size_t i = 3; i < bound; i += 2)
  {
    operations += 2; // For i < bound and i += 2
    if (numbers[i])
      ++counter;
  }

  cout << (counter + 1) << " primes in " << operations << " operations\n";
}