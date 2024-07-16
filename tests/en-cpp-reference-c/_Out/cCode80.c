//en.cppreference.com/w/c/language/array.html
#include <stdio.h>
 
int main(void)
{
   int n = 1;
label:;
   int a[n]; // re-allocated 10 times, each with a different size
   printf("The array has %zu elements\n", sizeof a / sizeof *a);
   if (n++ < 10)
       goto label; // leaving the scope of a VLA ends its lifetime
}