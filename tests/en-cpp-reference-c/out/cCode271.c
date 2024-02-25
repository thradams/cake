//en.cppreference.com/w/c/numeric/math/scalbn.html
#include <errno.h>
#include <fenv.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
 
// #pragma STDC FENV_ACCESS ON
 
int main(void)
{
    printf("scalbn(7, -4) = %f\n", scalbn(7, -4));
    printf("scalbn(1, -1074) = %g (minimum positive subnormal double)\n",
            scalbn(1, -1074));
    printf("scalbn(nextafter(1,0), 1024) = %g (largest finite double)\n",
            scalbn(nextafter(1,0), 1024));
 
    // special values
    printf("scalbn(-0, 10) = %f\n", scalbn(-0.0, 10));
    printf("scalbn(-Inf, -1) = %f\n", scalbn(-INFINITY, -1));
 
    // error handling
    errno = 0; feclearexcept(FE_ALL_EXCEPT);
    printf("scalbn(1, 1024) = %f\n", scalbn(1, 1024));
    if (errno == ERANGE)
        perror("    errno == ERANGE");
    if (fetestexcept(FE_OVERFLOW))
        puts("    FE_OVERFLOW raised");
}