//en.cppreference.com/w/c/numeric/math/roundeven.html
#include <math.h>
#include <stdio.h>
int main(void)
{
    printf("roundeven(+2.4) = %+.1f\n", roundeven(2.4));
    printf("roundeven(-2.4) = %+.1f\n", roundeven(-2.4));
    printf("roundeven(+2.5) = %+.1f\n", roundeven(2.5));
    printf("roundeven(-2.5) = %+.1f\n", roundeven(-2.5));
    printf("roundeven(+2.6) = %+.1f\n", roundeven(2.6));
    printf("roundeven(-2.6) = %+.1f\n", roundeven(-2.6));
    printf("roundeven(+3.5) = %+.1f\n", roundeven(3.5));
    printf("roundeven(-3.5) = %+.1f\n", roundeven(-3.5));
    printf("roundeven(-0.0) = %+.1f\n", roundeven(-0.0));
    printf("roundeven(-Inf) = %+f\n",   roundeven(-INFINITY));
}