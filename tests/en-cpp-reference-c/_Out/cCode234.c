//en.cppreference.com/w/c/numeric/math/floor.html
#include <math.h>
#include <stdio.h>
int main(void)
{
    printf("floor(+2.7) = %+.1f\n", floor(2.7));
    printf("floor(-2.7) = %+.1f\n", floor(-2.7));
    printf("floor(-0.0) = %+.1f\n", floor(-0.0));
    printf("floor(-Inf) = %+f\n",   floor(-INFINITY));
}