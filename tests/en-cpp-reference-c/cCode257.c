//en.cppreference.com/w/c/numeric/math/log1p.html
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <errno.h>
#include <fenv.h>
#pragma STDC FENV_ACCESS ON
int main(void)
{
    printf("log1p(0) = %f\n", log1p(0));
    printf("Interest earned in 2 days on $100, compounded daily at 1%%\n"
           " on a 30/360 calendar = %f\n",
           100*expm1(2*log1p(0.01/360)));
    printf("log(1+1e-16) = %g, but log1p(1e-16) = %g\n",
           log(1+1e-16), log1p(1e-16));
    // special values
    printf("log1p(-0) = %f\n", log1p(-0.0));
    printf("log1p(+Inf) = %f\n", log1p(INFINITY));
    //error handling
    errno = 0; feclearexcept(FE_ALL_EXCEPT);
    printf("log1p(-1) = %f\n", log1p(-1));
    if(errno == ERANGE) perror("    errno == ERANGE");
    if(fetestexcept(FE_DIVBYZERO)) puts("    FE_DIVBYZERO raised");
}