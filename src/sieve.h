#ifndef INCLUDED_SIEVE
#define INCLUDED_SIEVE

#include "../config.h"

ullong max(ullong a, ullong b);
ullong min(ullong a, ullong b);
void logLastPrimes(ullong counters[], ullong startsAt[]);
void sieve();

#endif
