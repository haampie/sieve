#ifndef INCLUDED_SIEVE
#define INCLUDED_SIEVE

unsigned long max(unsigned long a, unsigned long b);
unsigned long min(unsigned long a, unsigned long b);
void logLastPrimes(unsigned long counters[], unsigned long startsAt[]);
void sieve();

#endif