//en.cppreference.com/w/c/numeric/math/logb.html
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <fenv.h>
 
// #pragma STDC FENV_ACCESS ON
int main(void)
{
    double f = 123.45;
    printf("Given the number %.2f or %a in hex,\n", f, f);
 
    double f3;
    double f2 = modf(f, &f3);
    printf("modf() makes %.0f + %.2f\n", f3, f2);
 
    int i;
    f2 = frexp(f, &i);
    printf("frexp() makes %f * 2^%d\n", f2, i);
 
    i = logb(f);
    printf("logb()/logb() make %f * %d^%d\n", f/scalbn(1.0, i), FLT_RADIX, i);
 
    // error handling
    feclearexcept(FE_ALL_EXCEPT);
    printf("logb(0) = %f\n", logb(0));
    if(fetestexcept(FE_DIVBYZERO)) puts("    FE_DIVBYZERO raised");
}