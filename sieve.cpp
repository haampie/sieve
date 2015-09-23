#include <iostream>
#include <math.h>
#include <string>

using namespace std;

int main(int argc, char const *argv[])
{
	size_t primes_under = 100;
	size_t counter = 0;
	
	if(argc == 2)
		primes_under = stoi(argv[1]);

	bool numbers[primes_under];

	for(size_t i = 0; i < primes_under; ++i)
		numbers[i] = true;

	for(size_t i = 3; i <= sqrt(primes_under); i+=2)
		if(numbers[i])
			for(size_t j = i*i; j < primes_under; j += i)
				numbers[j] = false;

	for(size_t i = 3; i < primes_under; i+=2)
		if(numbers[i])
			++counter;
	
	cout << (counter+1) << '\n';

	return 0;
}