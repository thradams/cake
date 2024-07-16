//en.cppreference.com/w/c/numeric/math/sinh.html
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <fenv.h>
 
// #pragma STDC FENV_ACCESS ON
int main(void)
{
    printf("sinh(1) = %f\nsinh(-1)=%f\n", sinh(1), sinh(-1));
    printf("log(sinh(1) + cosh(1))=%f\n", log(sinh(1)+cosh(1)));
    // special values
    printf("sinh(+0) = %f\nsinh(-0)=%f\n", sinh(0.0), sinh(-0.0));
    // error handling 
    errno=0; feclearexcept(FE_ALL_EXCEPT);
    printf("sinh(710.5) = %f\n", sinh(710.5));
    if(errno == ERANGE) perror("    errno == ERANGE");
    if(fetestexcept(FE_OVERFLOW)) puts("    FE_OVERFLOW raised");
}