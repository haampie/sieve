#ifndef INCLUDE_CONFIG
#define INCLUDE_CONFIG

#include "./src/parallel.h"

typedef unsigned long long ullong;
typedef MCBSP_PROCESSOR_INDEX_DATATYPE processors;

enum N_PROGRAM {
  GENERATE,
  TWIN,
  GOLDBACH
};

#endif