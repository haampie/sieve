#include "main.h"
#include "config.h"
#include "src/get_primes_under_limit.h"
#include "src/parallel.h"

#include <iostream>

processors P = 4;
size_t limit = 1000000000;

int main(int argc, char ** argv)
{
  bsp_init(get_primes_under_limit, argc, argv);
  get_primes_under_limit();
}
