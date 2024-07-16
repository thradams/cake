//en.cppreference.com/w/c/numeric/math/sin.html
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
    printf("sin(pi/6) = %f\n", sin(pi / 6));
    printf("sin(pi/2) = %f\n", sin(pi / 2));
    printf("sin(-3*pi/4) = %f\n", sin(-3 * pi / 4));
 
    // special values
    printf("sin(+0) = %f\n", sin(0.0));
    printf("sin(-0) = %f\n", sin(-0.0));
 
    // error handling
    feclearexcept(FE_ALL_EXCEPT);
    printf("sin(INFINITY) = %f\n", sin(INFINITY));
    if (fetestexcept(FE_INVALID))
        puts("    FE_INVALID raised");
}