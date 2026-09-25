//en.cppreference.com/w/c/numeric/math/expm1.html
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <errno.h>
#include <fenv.h>
// #pragma STDC FENV_ACCESS ON
int main(void)
{
    printf("expm1(1) = %f\n", expm1(1));
    printf("Interest earned in 2 days on $100, compounded daily at 1%%\n"
           " on a 30/360 calendar = %f\n",
           100*expm1(2*log1p(0.01/360)));
    printf("exp(1e-16)-1 = %g, but expm1(1e-16) = %g\n",
           exp(1e-16)-1, expm1(1e-16));
    // special values
    printf("expm1(-0) = %f\n", expm1(-0.0));
    printf("expm1(-Inf) = %f\n", expm1(-INFINITY));
    //error handling
    errno = 0; feclearexcept(FE_ALL_EXCEPT);
    printf("expm1(710) = %f\n", expm1(710));
    if(errno == ERANGE) perror("    errno == ERANGE");
    if(fetestexcept(FE_OVERFLOW)) puts("    FE_OVERFLOW raised");
}