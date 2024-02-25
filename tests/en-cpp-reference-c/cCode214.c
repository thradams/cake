//en.cppreference.com/w/c/numeric/math/asin.html
#include <errno.h>
#include <fenv.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
 
#ifndef __GNUC__
#pragma STDC FENV_ACCESS ON
#endif
 
int main(void)
{
    printf("asin( 1.0) = %+f, 2*asin( 1.0)=%+f\n", asin(1), 2 * asin(1));
    printf("asin(-0.5) = %+f, 6*asin(-0.5)=%+f\n", asin(-0.5), 6 * asin(-0.5));
 
    // special values
    printf("asin(0.0) = %1f, asin(-0.0)=%f\n", asin(+0.0), asin(-0.0));
 
    // error handling
    errno = 0; feclearexcept(FE_ALL_EXCEPT);
    printf("asin(1.1) = %f\n", asin(1.1));
    if (errno == EDOM)
        perror("    errno == EDOM");
    if (fetestexcept(FE_INVALID))
        puts("    FE_INVALID raised");
}