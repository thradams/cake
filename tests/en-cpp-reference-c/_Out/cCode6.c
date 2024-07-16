//en.cppreference.com/w/c/chrono/clock_t.html
#include <stdio.h>
#include <time.h>
#include <math.h>
 
volatile double sink;
int main (void)
{
  clock_t start = clock();
 
  for(size_t i=0; i<3141592; ++i)
      sink+=sin(i);
 
  clock_t end = clock();
  double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
 
  printf("for loop took %f seconds to execute \n", cpu_time_used);
}