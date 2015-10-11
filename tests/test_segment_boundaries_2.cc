#include <iostream>

#include "../src/segment_boundaries_2.h"
#include "../config.h"

using namespace std;

int main(int argc, char const *argv[])
{
  size_t from;
  size_t to;

  size_t start = 0;
  size_t end = 1000;

  processors P = 4;

  for(processors i = 0; i < 4; ++i)
  {
    segmentBoundariesTryTwo(&from, &to, start, end, P, i);
    cout << from << " - " << to << '\n';
  }

  return 0;
}