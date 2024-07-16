//en.cppreference.com/w/c/numeric/math/modf.html
#include <stdio.h>
#include <math.h>
#include <float.h>
 
int main(void)
{
    double f = 123.45;
    printf("Given the number %.2f or %a in hex,\n", f, f);
 
    double f3;
    double f2 = modf(f, &f3);
    printf("modf() makes %.2f + %.2f\n", f3, f2);
 
    int i;
    f2 = frexp(f, &i);
    printf("frexp() makes %f * 2^%d\n", f2, i);
 
    i = ilogb(f);
    printf("logb()/ilogb() make %f * %d^%d\n", f/scalbn(1.0, i), FLT_RADIX, i);
 
    // special values
    f2 = modf(-0.0, &f3);
    printf("modf(-0) makes %.2f + %.2f\n", f3, f2);
    f2 = modf(-INFINITY, &f3);
    printf("modf(-Inf) makes %.2f + %.2f\n", f3, f2);
}