//en.cppreference.com/w/c/numeric/math/sinpi.html
#include <errno.h>
#include <fenv.h>
#include <math.h>
#include <stdio.h>
 
#ifndef __GNUC__
#pragma STDC FENV_ACCESS ON
#endif
 
#if __STDC_VERSION__ < 202311L
// A naive implementation of a subset of sinpi family
double sinpi(double arg)
{
    return sin(arg * (double)3.1415926535897932384626433);
}
#endif
 
int main(void)
{
    const double pi = acos(-1);
 
    // typical usage
    printf("sinpi(1) = %f, sin(pi) = %f\n", sinpi(1), sin(pi));
    printf("sinpi(0.5) = %f, sin(pi/2) = %f\n", sinpi(0.5), sin(pi / 2));
    printf("sinpi(-0.75) = %f, sin(-3*pi/4) = %f\n", sinpi(-0.75), sin(-3 * pi / 4));
 
    // special values
    printf("sinpi(+0) = %f\n", sinpi(0.0));
    printf("sinpi(-0) = %f\n", sinpi(-0.0));
 
    // error handling
    feclearexcept(FE_ALL_EXCEPT);
    printf("sinpi(INFINITY) = %f\n", sinpi(INFINITY));
    if (fetestexcept(FE_INVALID))
        puts("    FE_INVALID raised");
}