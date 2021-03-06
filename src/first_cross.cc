#include "first_cross.h"

size_t firstCross(size_t s, size_t low) {
  if (s * s > low)
    return (s * s - low);
  if ((low - s * s) % (2 * s) == 0)
    return 0;
  return (2 * s - ((low - s * s) % (2 * s)));
}