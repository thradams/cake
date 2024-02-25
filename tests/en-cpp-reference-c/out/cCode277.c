//en.cppreference.com/w/c/numeric/math/tan.html
#include <errno.h>
#include <fenv.h>
#include <math.h>
#include <stdio.h>
 
#ifndef __GNUC__
#pragma STDC FENV_ACCESS ON
#endif
 
int main(void)
{
    const double pi = acos(-1);
 
    // typical usage
    printf("tan(pi*1/4) = %+f\n", tan(pi * 1 / 4)); //   45 deg
    printf("tan(pi*3/4) = %+f\n", tan(pi * 3 / 4)); //  135 deg
    printf("tan(pi*5/4) = %+f\n", tan(pi * 5 / 4)); // -135 deg
    printf("tan(pi*7/4) = %+f\n", tan(pi * 7 / 4)); //  -45 deg
 
    // special values
    printf("tan(+0) = %f\n", tan(0.0));
    printf("tan(-0) = %f\n", tan(-0.0));
 
    // error handling
    feclearexcept(FE_ALL_EXCEPT);
    printf("tan(INFINITY) = %f\n", tan(INFINITY));
    if (fetestexcept(FE_INVALID))
        puts("    FE_INVALID raised");
}