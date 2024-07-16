//en.cppreference.com/w/c/numeric/math/fma.html
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <fenv.h>
#pragma STDC FENV_ACCESS ON
int main(void)
{
    // demo the difference between fma and built-in operators
    double in = 0.1;
    printf("0.1 double is %.23f (%a)\n", in, in);
    printf("0.1*10 is 1.0000000000000000555112 (0x8.0000000000002p-3),"
           " or 1.0 if rounded to double\n");
    double expr_result = 0.1 * 10 - 1;
    printf("0.1 * 10 - 1 = %g : 1 subtracted after "
           "intermediate rounding to 1.0\n", expr_result);
    double fma_result = fma(0.1, 10, -1);
    printf("fma(0.1, 10, -1) = %g (%a)\n", fma_result, fma_result);
 
    // fma use in double-double arithmetic
    printf("\nin double-double arithmetic, 0.1 * 10 is representable as ");
    double high = 0.1 * 10;
    double low = fma(0.1, 10, -high);
    printf("%g + %g\n\n", high, low);
 
    //error handling
    feclearexcept(FE_ALL_EXCEPT);
    printf("fma(+Inf, 10, -Inf) = %f\n", fma(INFINITY, 10, -INFINITY));
    if(fetestexcept(FE_INVALID)) puts("    FE_INVALID raised");
}