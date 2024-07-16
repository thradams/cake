//en.cppreference.com/w/c/numeric/math/log.html
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <errno.h>
#include <fenv.h>
// #pragma STDC FENV_ACCESS ON
int main(void)
{
    printf("log(1) = %f\n", log(1));
    printf("base-5 logarithm of 125 = %f\n", log(125)/log(5));
    // special values
    printf("log(1) = %f\n", log(1));
    printf("log(+Inf) = %f\n", log(INFINITY));
    //error handling
    errno = 0; feclearexcept(FE_ALL_EXCEPT);
    printf("log(0) = %f\n", log(0));
    if(errno == ERANGE) perror("    errno == ERANGE");
    if(fetestexcept(FE_DIVBYZERO)) puts("    FE_DIVBYZERO raised");
}