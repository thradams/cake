//en.cppreference.com/w/c/numeric/math/cos.html
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
    printf("cos(pi/3) = %f\n", cos(pi / 3));
    printf("cos(pi/2) = %f\n", cos(pi / 2));
    printf("cos(-3*pi/4) = %f\n", cos(-3 * pi / 4));
 
    // special values
    printf("cos(+0) = %f\n", cos(0.0));
    printf("cos(-0) = %f\n", cos(-0.0));
 
    // error handling
    feclearexcept(FE_ALL_EXCEPT);
    printf("cos(INFINITY) = %f\n", cos(INFINITY));
    if (fetestexcept(FE_INVALID))
        puts("    FE_INVALID raised");
}