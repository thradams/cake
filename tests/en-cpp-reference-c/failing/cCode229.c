//en.cppreference.com/w/c/numeric/math/exp2.html
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <errno.h>
#include <fenv.h>
// #pragma STDC FENV_ACCESS ON
int main(void)
{
    printf("exp2(5) = %f\n", exp2(5));
    printf("exp2(0.5) = %f\n", exp2(0.5));
    printf("exp2(-4) = %f\n", exp2(-4));
    // special values
    printf("exp2(-0.9) = %f\n", exp2(-0.9));
    printf("exp2(-Inf) = %f\n", exp2(-INFINITY));
    //error handling
    errno = 0; feclearexcept(FE_ALL_EXCEPT);
    printf("exp2(1024) = %f\n", exp2(1024));
    if(errno == ERANGE) perror("    errno == ERANGE");
    if(fetestexcept(FE_OVERFLOW)) puts("    FE_OVERFLOW raised");
}