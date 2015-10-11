#include "../config.h"

#include <iostream>
#include <cmath>

using namespace std;

void segmentBoundariesTryTwo(size_t *from, size_t *to, size_t rangeStart, size_t rangeEnd, processors P, processors index)
{
  *from = rangeStart;
  
  size_t width = (rangeEnd - rangeStart + 1);

  for(processors i = 1; i <= index; ++i)
  {
    width /= 2;
    *from += width;
  }

  if(index == P-1)
    *to = rangeEnd;
  else
    *to = *from + width/2 - 1;
}