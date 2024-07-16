//en.cppreference.com/w/c/numeric/math/round.html
#include <stdio.h>
#include <math.h>
#include <fenv.h>
#include <limits.h>
 
// #pragma STDC FENV_ACCESS ON
 
int main(void)
{
    // round
    printf("round(+2.3) = %+.1f  ", round(2.3));
    printf("round(+2.5) = %+.1f  ", round(2.5));
    printf("round(+2.7) = %+.1f\n", round(2.7));
    printf("round(-2.3) = %+.1f  ", round(-2.3));
    printf("round(-2.5) = %+.1f  ", round(-2.5));
    printf("round(-2.7) = %+.1f\n", round(-2.7));
 
    printf("round(-0.0) = %+.1f\n", round(-0.0));
    printf("round(-Inf) = %+f\n",   round(-INFINITY));
 
    // lround
    printf("lround(+2.3) = %ld  ", lround(2.3));
    printf("lround(+2.5) = %ld  ", lround(2.5));
    printf("lround(+2.7) = %ld\n", lround(2.7));
    printf("lround(-2.3) = %ld  ", lround(-2.3));
    printf("lround(-2.5) = %ld  ", lround(-2.5));
    printf("lround(-2.7) = %ld\n", lround(-2.7));
 
    printf("lround(-0.0) = %ld\n", lround(-0.0));
    printf("lround(-Inf) = %ld\n", lround(-INFINITY)); // FE_INVALID raised
 
    // error handling
    feclearexcept(FE_ALL_EXCEPT);
    printf("lround(LONG_MAX+1.5) = %ld\n", lround(LONG_MAX+1.5));
    if(fetestexcept(FE_INVALID)) puts("    FE_INVALID was raised");
}