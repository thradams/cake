//en.cppreference.com/w/c/numeric/math/hypot.html
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <fenv.h>
#include <float.h>
 
// #pragma STDC FENV_ACCESS ON
int main(void)
{
    // typical usage
    printf("(1,1) cartesian is (%f,%f) polar\n", hypot(1,1), atan2(1,1));
    // special values
    printf("hypot(NAN,INFINITY) = %f\n", hypot(NAN,INFINITY));
    // error handling 
    errno = 0; feclearexcept(FE_ALL_EXCEPT);
    printf("hypot(DBL_MAX,DBL_MAX) = %f\n", hypot(DBL_MAX,DBL_MAX));
    if(errno == ERANGE)         perror("    errno == ERANGE");
    if(fetestexcept(FE_OVERFLOW)) puts("    FE_OVERFLOW raised");
}