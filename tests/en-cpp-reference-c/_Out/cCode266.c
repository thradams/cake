//en.cppreference.com/w/c/numeric/math/remainder.html
#include <stdio.h>
#include <math.h>
#include <fenv.h>
 
#pragma STDC FENV_ACCESS ON
int main(void)
{
    printf("remainder(+5.1, +3.0) = %.1f\n", remainder(5.1,3));
    printf("remainder(-5.1, +3.0) = %.1f\n", remainder(-5.1,3));
    printf("remainder(+5.1, -3.0) = %.1f\n", remainder(5.1,-3));
    printf("remainder(-5.1, -3.0) = %.1f\n", remainder(-5.1,-3));
 
    // special values
    printf("remainder(-0.0, 1.0) = %.1f\n", remainder(-0.0, 1));
    printf("remainder(+5.1, Inf) = %.1f\n", remainder(5.1, INFINITY));
 
    // error handling
    feclearexcept(FE_ALL_EXCEPT);
    printf("remainder(+5.1, 0) = %.1f\n", remainder(5.1, 0));
    if(fetestexcept(FE_INVALID)) puts("    FE_INVALID raised");
}