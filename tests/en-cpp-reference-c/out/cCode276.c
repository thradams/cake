//en.cppreference.com/w/c/numeric/math/sqrt.html
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <fenv.h>
 
#pragma STDC FENV_ACCESS ON
 
int main(void)
{
    // normal use
    printf("sqrt(100) = %f\n", sqrt(100));
    printf("sqrt(2) = %f\n", sqrt(2));
    printf("golden ratio = %f\n", (1+sqrt(5))/2);
    // special values
    printf("sqrt(-0) = %f\n", sqrt(-0.0));
    // error handling
    errno = 0; feclearexcept(FE_ALL_EXCEPT);
    printf("sqrt(-1.0) = %f\n", sqrt(-1));
    if(errno == EDOM) perror("    errno == EDOM");
    if(fetestexcept(FE_INVALID)) puts("    FE_INVALID was raised");
}