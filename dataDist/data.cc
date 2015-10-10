#include <iostream>
#include <math.h>
#include <stdio.h>

int P = 8;
int N = 1000000;

int xInitial(int core){
  return core*(N/P);
}

int main(){
  int startAt[P+1];

  for (int i=0;i<=P;i++){
    startAt[i] = xInitial(i);
  }

  bool x[N];
  for (int i=0;i<N;i++){
    x[i] = true;
  }

  x[0] = false;
  x[1] = false;

  int crossCount[P];
  
  int j = 0, k = 0;
  int lim = sqrt(N) + 1;

  while (j++ <= lim){
    if (x[j])
      k = 0;
      for (int i=j*j;i<N;i+=j)
	{
	  while (startAt[k+1] < i)
	    k++;	  
	  x[i] = false;
	  crossCount[k]++;
	}
  }

  printf("\n");

  for (j=0;j<P;j++){
    printf("crossCount[%d] = %d\n",j,crossCount[j] );
  }
  
  return 0; 
}
