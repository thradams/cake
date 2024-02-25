//en.cppreference.com/w/c/numeric/math/remquo.html
#include <stdio.h>
#include <math.h>
#include <fenv.h>
#ifndef __GNUC__
#pragma STDC FENV_ACCESS ON
#endif
double cos_pi_x_naive(double x)
{
    const double pi = acos(-1);
    return cos(pi * x);
}
// the period is 2, values are (0;0.5) positive, (0.5;1.5) negative, (1.5,2) positive
double cos_pi_x_smart(double x)
{
    const double pi = acos(-1);
    int extremum;
    double rem = remquo(x, 1, &extremum);
    extremum = (unsigned)extremum % 2; // keep 1 bit to determine nearest extremum
    return extremum ? -cos(pi * rem) : cos(pi * rem);
}
int main(void)
{
    printf("cos(pi * 0.25) = %f\n", cos_pi_x_naive(0.25));
    printf("cos(pi * 1.25) = %f\n", cos_pi_x_naive(1.25));
    printf("cos(pi * 1000000000000.25) = %f\n", cos_pi_x_naive(1000000000000.25));
    printf("cos(pi * 1000000000001.25) = %f\n", cos_pi_x_naive(1000000000001.25));
    printf("cos(pi * 1000000000000.25) = %f\n", cos_pi_x_smart(1000000000000.25));
    printf("cos(pi * 1000000000001.25) = %f\n", cos_pi_x_smart(1000000000001.25));
    // error handling
    feclearexcept(FE_ALL_EXCEPT);
    int quo;
    printf("remquo(+Inf, 1) = %.1f\n", remquo(INFINITY, 1, &quo));
    if(fetestexcept(FE_INVALID)) puts("    FE_INVALID raised");
}