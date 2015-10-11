#include "main.h"
#include "config.h"
#include "src/segmented_sieve.h"
#include "src/parallel.h"

#include <iostream>

processors P = 4;
size_t limit = 1000000000;

int main(int argc, char ** argv)
{
  bsp_init(segmented_sieve, argc, argv);
  segmented_sieve();
}
