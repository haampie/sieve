#include "check_twin.h"

#include <iostream>
#include <cmath>

using namespace std;

void checkTwin(vector<size_t>* twins, processors P)
{
  processors core = bsp_pid();
 
  for (processors i = 0; i < P; ++i)
  {
    if (core == i) 
      for(size_t j = 0; j < twins->size(); ++j)
        cout << (*twins)[j] - 2 << " " << (*twins)[j] << '\n';

    bsp_sync();
  }
}
