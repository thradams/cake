//en.cppreference.com/w/c/numeric/math/trunc.html
#include <math.h>
#include <stdio.h>
int main(void)
{
    printf("trunc(+2.7) = %+.1f\n", trunc(2.7));
    printf("trunc(-2.7) = %+.1f\n", trunc(-2.7));
    printf("trunc(-0.0) = %+.1f\n", trunc(-0.0));
    printf("trunc(-Inf) = %+f\n",   trunc(-INFINITY));
}