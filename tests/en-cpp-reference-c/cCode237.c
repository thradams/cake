//en.cppreference.com/w/c/numeric/math/fmin.html
#include <stdio.h>
#include <math.h>
 
int main(void)
{
    printf("fmin(2,1)    = %f\n", fmin(2,1));
    printf("fmin(-Inf,0) = %f\n", fmin(-INFINITY,0));
    printf("fmin(NaN,-1) = %f\n", fmin(NAN,-1));
}