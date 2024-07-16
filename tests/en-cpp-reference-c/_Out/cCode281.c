//en.cppreference.com/w/c/numeric/random/RAND_MAX.html
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
 
int main(void)
{
    srand(time(NULL)); // use current time as seed for random generator
    printf("RAND_MAX: %i\n", RAND_MAX);
    printf("INT_MAX: %i\n", INT_MAX);
    printf("Random value on [0,1]: %f\n", (double)rand() / RAND_MAX);
}