//en.cppreference.com/w/c/numeric/math/log2.html
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <errno.h>
#include <fenv.h>
// #pragma STDC FENV_ACCESS ON
int main(void)
{
    printf("log2(65536) = %f\n", log2(65536));
    printf("log2(0.125) = %f\n", log2(0.125));
    printf("log2(0x020f) = %f (highest set bit is in position 9)\n", log2(0x020f));
    printf("base-5 logarithm of 125 = %f\n", log2(125)/log2(5));
    // special values
    printf("log2(1) = %f\n", log2(1));
    printf("log2(+Inf) = %f\n", log2(INFINITY));
    //error handling
    errno = 0; feclearexcept(FE_ALL_EXCEPT);
    printf("log2(0) = %f\n", log2(0));
    if(errno == ERANGE) perror("    errno == ERANGE");
    if(fetestexcept(FE_DIVBYZERO)) puts("    FE_DIVBYZERO raised");
}