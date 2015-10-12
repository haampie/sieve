#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include <stdio.h>

using namespace std;

void goldbach(vector<size_t>* primes, size_t bound, size_t n_print) {

	vector<char> checkGB((bound - 1) / 2 + 1, 0);
	// Fill a vector of length (bound-1)/2 with false.  These bools represent all the even numbers
	// 2,4,6,..,bound. Then we will see if all   these may be written as a sum of two (odd) primes.
	// The number 2m corresponds to index m - 1 in checkGB. Similarly to the number n corresponds
	// the index n/2 - 1.

	size_t goldbach_pairs[n_print][2];

	size_t numberOfPrimes = primes->size(); // size for the goldbach part

	if ( n_print >= bound / 2 - 1) { // make sure we are not asking to print too many arguments
		printf("4 = 2 + 2\n");
		n_print = bound / 2 - 2;
	}

	size_t evenNumbersToBeChecked = bound / 2 - 2;
	bool verified = false;

	// In this loop we check to see if all even numbers can we written as a sum of two primes. This
	// is done by looping through the primes and crossing off the

	size_t printingBound = bound - 2 * n_print;

	for (size_t lhs = 0; lhs < numberOfPrimes; ++lhs)
	{
		if(verified)
			break;

		for (size_t rhs = lhs; rhs < numberOfPrimes; ++rhs)
		{
			size_t sum = (*primes)[lhs] + (*primes)[rhs];

			if (sum > bound)
				break;

			if ( not checkGB[ sum / 2 - 1 ] )
			{
				if(sum > printingBound)
				{
					goldbach_pairs[ n_print - 1 + ((*primes)[lhs] + (*primes)[rhs] - bound) / 2  ][0] = (*primes)[lhs];
					goldbach_pairs[ n_print - 1 + ((*primes)[lhs] + (*primes)[rhs] - bound) / 2  ][1] = (*primes)[rhs];
				}

				if(--evenNumbersToBeChecked == 0)
				{
					verified = true;
					break;
				}
			}

			checkGB[ sum / 2 - 1 ] = 1;
		}
	}

	for (size_t i = 0; i < n_print; ++i)
		printf("%lu = %lu + %lu\n", goldbach_pairs[i][1] + goldbach_pairs[i][0], goldbach_pairs[i][0], goldbach_pairs[i][1]);

  if(not verified)
  	printf("The Goldbach conjecture has been falsified\n");
}
