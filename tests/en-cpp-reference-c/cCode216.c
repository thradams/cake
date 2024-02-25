//en.cppreference.com/w/c/numeric/math/atan.html
#include <math.h>
#include <stdio.h>
 
int main(void)
{
    printf("atan(1) = %f, 4*atan(1)=%f\n", atan(1), 4 * atan(1));
    // special values
    printf("atan(Inf) = %f, 2*atan(Inf) = %f\n", atan(INFINITY), 2 * atan(INFINITY));
    printf("atan(-0.0) = %+f, atan(+0.0) = %+f\n", atan(-0.0), atan(0));
}