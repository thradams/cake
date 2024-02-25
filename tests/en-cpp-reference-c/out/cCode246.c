//en.cppreference.com/w/c/numeric/math/isinf.html
#include <stdio.h>
#include <math.h>
#include <float.h>
 
int main(void)
{
    printf("isinf(NAN)         = %d\n", isinf(NAN));
    printf("isinf(INFINITY)    = %d\n", isinf(INFINITY));
    printf("isinf(0.0)         = %d\n", isinf(0.0));
    printf("isinf(DBL_MIN/2.0) = %d\n", isinf(DBL_MIN/2.0));
    printf("isinf(1.0)         = %d\n", isinf(1.0));
    printf("isinf(exp(800))    = %d\n", isinf(exp(800)));
}