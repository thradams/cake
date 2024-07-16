//en.cppreference.com/w/c/numeric/math/cbrt.html
#include <stdio.h>
#include <float.h>
#include <math.h>
 
int main(void)
{
    printf("Normal use:\n"
           "cbrt(729)      = %f\n", cbrt(729));
    printf("cbrt(-0.125)   = %f\n", cbrt(-0.125));
    printf("Special values:\n"
           "cbrt(-0)       = %f\n", cbrt(-0.0));
    printf("cbrt(+inf)     = %f\n", cbrt(INFINITY));
    printf("Accuracy:\n"
           "cbrt(343)      = %.*f\n", DBL_DECIMAL_DIG, cbrt(343));
    printf("pow(343,1.0/3) = %.*f\n", DBL_DECIMAL_DIG, pow(343, 1.0/3));
}