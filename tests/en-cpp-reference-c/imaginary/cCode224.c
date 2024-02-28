//en.cppreference.com/w/c/numeric/math/cospi.html
#include <errno.h>
#include <fenv.h>
#include <math.h>
#include <stdio.h>
 
#ifndef __GNUC__
#pragma STDC FENV_ACCESS ON
#endif
 
#if __STDC_VERSION__ < 202311L
// A naive implementation of a subset of cospi family
double cospi(double arg)
{
    return cos(arg * (double)3.1415926535897932384626433);
}
#endif
 
int main(void)
{
    const double pi = acos(-1);
 
    // typical usage
    printf("cospi(1) = %f, cos(pi) = %f\n", cospi(1), cos(pi));
    printf("cospi(0.5) = %f, cos(pi/2) = %f\n", cospi(0.5), cos(pi / 2));
    printf("cospi(-0.75) = %f, cos(-3*pi/4) = %f\n", cospi(-0.75), cos(-3 * pi / 4));
 
    // special values
    printf("cospi(+0) = %f\n", cospi(0.0));
    printf("cospi(-0) = %f\n", cospi(-0.0));
 
    // error handling
    feclearexcept(FE_ALL_EXCEPT);
    printf("cospi(INFINITY) = %f\n", cospi(INFINITY));
    if (fetestexcept(FE_INVALID))
        puts("    FE_INVALID raised");
}