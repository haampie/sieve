#include "../config.h"

#include <iostream>
#include <cmath>

using namespace std;

void segmentBoundaries(size_t *from, size_t *to, size_t rangeStart, size_t rangeEnd, processors P, processors index)
{
  // When working with 1 prime this is easy
  if (P == 1)
  {
    *from = rangeStart;
    *to = rangeEnd;
    return;
  }

  size_t width = (rangeEnd - rangeStart + 1);
  size_t remains = width % P;
  size_t part = width / P;

  *from = rangeStart + index * part;
  *to = *from + part - 1;

  if(index == P-1)
  {
    *to = rangeEnd;  
  }
}