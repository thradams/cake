//en.cppreference.com/w/c/numeric/math/lgamma.html
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <errno.h>
#include <fenv.h>
// #pragma STDC FENV_ACCESS ON
 
int main(void)
{
    printf("lgamma(10) = %f, log(9!) = %f\n", lgamma(10), log(2*3*4*5*6*7*8*9));
    const double pi = acos(-1);
    printf("lgamma(0.5) = %f, log(sqrt(pi)) = %f\n", log(sqrt(pi)), lgamma(0.5));
    // special values
    printf("lgamma(1) = %f\n", lgamma(1));
    printf("lgamma(+Inf) = %f\n", lgamma(INFINITY));
 
    // error handling
    errno = 0; feclearexcept(FE_ALL_EXCEPT);
    printf("lgamma(0) = %f\n", lgamma(0));
    if (errno == ERANGE)
        perror("    errno == ERANGE");
    if (fetestexcept(FE_DIVBYZERO))
        puts("    FE_DIVBYZERO raised");
}