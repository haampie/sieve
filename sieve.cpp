#include <iostream>
#include <math.h>
#include <vector>
#include <string>

using namespace std;

int main(int argc, char const *argv[])
{
	size_t limit = 1000000000;
	size_t halfLimit = limit / 2;
	size_t sqrtPrimes = sqrt(limit)/2;
	size_t counter = 0;

	vector<bool> numbers(halfLimit, true);

	for (size_t i = 1; i <= sqrtPrimes; ++i)
	{
		if (numbers[i])
		{
			// n^2 in nieuwe index is => (n^2-1)/2 = ((4j+1)^2-1)/2 = 2(j*j+j)
			for (size_t j = 2*(i*i+i); j < halfLimit; j += 2*i+1)
				numbers[j] = false;
		}
	}

	for (size_t i = 1; i < halfLimit; ++i)
		if (numbers[i])
			++counter;

	cout << (counter + 1) << " primes <= " << limit << '\n';

	return 0;
}