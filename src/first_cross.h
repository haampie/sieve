#ifndef __INCLUDE_CHECKTWIN__
#define __INCLUDE_CHECKTWIN__

int firstCross(int s, int low) {
  if (s * s > low)
    return (s * s - low);
  if ((low - s * s) % (2 * s) == 0)
    return 0;
  return (2 * s - ((low - s * s) % (2 * s)));
}

#endif