//en.cppreference.com/w/c/numeric/math/fmax.html
#include <stdio.h>
#include <math.h>
 
int main(void)
{
    printf("fmax(2,1)    = %f\n", fmax(2,1));
    printf("fmax(-Inf,0) = %f\n", fmax(-INFINITY,0));
    printf("fmax(NaN,-1) = %f\n", fmax(NAN,-1));
}