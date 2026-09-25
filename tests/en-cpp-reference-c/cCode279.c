//en.cppreference.com/w/c/numeric/math/tgamma.html
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <errno.h>
#include <fenv.h>
// #pragma STDC FENV_ACCESS ON
 
int main(void)
{
    printf("tgamma(10) = %f, 9!=%f\n", tgamma(10), 2 * 3 * 4 * 5 * 6 * 7 * 8 * 9.0);
    printf("tgamma(0.5) = %f, sqrt(pi) = %f\n", sqrt(acos(-1)), tgamma(0.5));
 
    // special values
    printf("tgamma(+Inf) = %f\n", tgamma(INFINITY));
 
    // error handling
    errno = 0; feclearexcept(FE_ALL_EXCEPT);
    printf("tgamma(-1) = %f\n", tgamma(-1));
    if (errno == ERANGE)
        perror("    errno == ERANGE");
    else
        if (errno == EDOM)   perror("    errno == EDOM");
    if (fetestexcept(FE_DIVBYZERO))
        puts("    FE_DIVBYZERO raised");
    else if (fetestexcept(FE_INVALID))
        puts("    FE_INVALID raised");
}