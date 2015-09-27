#include <stdio.h>
#include <math.h>
#include "bspedupack.h"

#define bool  _Bool
#define true  1
#define false 0

unsigned long N = 100;
size_t P;
unsigned long nPrint;

unsigned long max(unsigned long a, unsigned long b){
  return (a>b) ? a : b;
}

unsigned long min(unsigned long a, unsigned long b){
  return (a<b) ? a : b;
}

void sieve() {

  bsp_begin(P);

  double time0, time1;
  time0 = bsp_time();

  size_t core = bsp_pid();

  // Distribution: proc 0 gets 0, 1, ..., sqrt(N)
  // proc 1, ..., p get the remainders evenly

  /******* Find sizes for each proc and initialize arrays *********/

  unsigned long sizes[P];
  unsigned long startsAt[P];

  sizes[0] = max((unsigned long) sqrt(N) + 1, (unsigned long) (N / P));
  startsAt[0] = 0;

  unsigned long size = (N - sizes[0] + 1) / (P - 1);
  unsigned long remains = (N - sizes[0] + 1) % (P - 1);

  for (size_t index = 1; index < P; ++index)
    {
      sizes[index] = size;

      if (index <= remains)
	++sizes[index];

      startsAt[index] = startsAt[index - 1] + sizes[index - 1];
    }

  /* if(core == 0) */
  /*   for(size_t p = 0; p < P; ++p) */
  /*     printf("Core %zu gets %lu to %lu\n", p, startsAt[p], startsAt[p]+sizes[p]-1); */

  bool x[sizes[core]];
  unsigned long prime = 2;

  bsp_push_reg(&prime, sizeof(unsigned long));
  bsp_sync();

  for (unsigned long index = 0; index < sizes[core]; ++index)
    x[index] = true;

  /******************* Start the sieve process *************/

  unsigned long L = min(sizes[0],sqrt(N)+1);
  while (prime < L)
    {  
      if (core == 0)
	{
	  // Find the next prime under sqrt(N)
        
	  while (!x[prime]){
	    prime ++;
	  }

	  // Send it to the others
	  for (size_t proc = 1; proc < P; ++proc)
	    bsp_put(proc, &prime, &prime, 0, sizeof(unsigned long));
	}
      bsp_sync();
  
      // Sieve all cores
      for (unsigned long multiple = prime * prime; multiple < startsAt[core] + sizes[core]; multiple += prime)
	x[multiple - startsAt[core]] = false;
      
      prime ++;
    }

  /************* Count the primes in each core and find the total *********/
  
  unsigned long count = 0;

  for (unsigned long index = 0; index < sizes[core]; ++index)
    if (x[index] == true && !((core==0) && (index<2)))
      ++count;

  // Send the counters to all processors
  unsigned long counters[P];
  bsp_push_reg(counters,P*(sizeof(unsigned long)));
  bsp_sync();

  for (int i=0;i<P;i++)
    bsp_put(i,&count,counters,core*sizeof(unsigned long),sizeof(unsigned long));

  bsp_sync();
  
  for (int i=(P-1);i>0;i--)
    counters[i-1] += counters[i];

  /* if (core==0) */
  /*   for (i=0;i<(P-1);i++) */
  /*     printf("counter[%d]=%zu\n",i,counters[i]); */
  
  if (core==0)
    printf("there are %zu primes in [%d...%zu]. The last %zu are:\n",counters[0],2,N,min(nPrint,counters[0]));

  /************** Print the last nPrint primes **************/
  
  int printSkip[P];
  for (int i=0;i<P;i++)
    printSkip[i] = 0;
 
  int printCore; // Core where the first prime to be printed is
  
  if (nPrint >= counters[0])
    printCore = 0;
  else
    for (int i=(P-1);i>=0;i--)
      if (counters[i] >= nPrint)
	{
	  printSkip[i] = counters[i] - nPrint; // How many primes in printCore we need to skip
	  printCore = i;
	  break;
	}

  bsp_sync();

  for (int i=printCore;i<P;i++)
    {
      if (core == i)
	for (unsigned long j=0;j<sizes[core];j++)
	  if (x[j] && !((core==0)&&(j<2)))
	    {
	      if (printSkip[core] == 0)
		printf("%zu\n",startsAt[core] + j);
	      else
		printSkip[core] --;
	    }
      bsp_sync();
    }

  /************* Find the time taken ***********/
  
  time1 = bsp_time();
  if (core == 0)
    printf("Time taken %f\n",time1-time0);
  //  printf("Proc %zu found %zu primes\n", core, count);
  bsp_end();

  /************ FIN ****************************/
}

int main(int argc, char ** argv)
{
  bsp_init(sieve, argc, argv);

  printf("How many processors do you want to use?\n");
  fflush(stdout);

  scanf("%zu", &P);

  if (P > bsp_nprocs())
    {
      printf("Sorry, not enough processors available.\n");
      fflush(stdout);
      exit(1);
    }

  printf("The last how many primes need to be printed?\n");
  fflush(stdout);

  scanf("%zu", &nPrint);

  if (nPrint < 0)
    {
      printf("Cannot print a negative amount of primes.\n");
      fflush(stdout);
      exit(1);
    }
  
  if (P <= 1)
    {
      printf("Select at least two cores.\n");
      fflush(stdout);
      exit(1);
    }

  printf("Upper bound?\n");
  fflush(stdout);
  scanf("%lu", &N);

  sieve();

  //  printf("\n");
}
