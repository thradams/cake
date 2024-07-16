//en.cppreference.com/w/c/numeric/math/pow.html
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <fenv.h>
 
#pragma STDC FENV_ACCESS ON
int main(void)
{
    // typical usage
    printf("pow(2, 10) = %f\n", pow(2,10));
    printf("pow(2, 0.5) = %f\n", pow(2,0.5));
    printf("pow(-2, -3) = %f\n", pow(-2,-3));
    // special values
    printf("pow(-1, NAN) = %f\n", pow(-1,NAN));
    printf("pow(+1, NAN) = %f\n", pow(+1,NAN));
    printf("pow(INFINITY, 2) = %f\n", pow(INFINITY, 2));
    printf("pow(INFINITY, -1) = %f\n", pow(INFINITY, -1));
    // error handling 
    errno = 0; feclearexcept(FE_ALL_EXCEPT);
    printf("pow(-1, 1/3) = %f\n", pow(-1, 1.0/3));
    if(errno == EDOM)         perror("    errno == EDOM");
    if(fetestexcept(FE_INVALID)) puts("    FE_INVALID raised");
 
    feclearexcept(FE_ALL_EXCEPT);
    printf("pow(-0, -3) = %f\n", pow(-0.0, -3));
    if(fetestexcept(FE_DIVBYZERO)) puts("    FE_DIVBYZERO raised");
}