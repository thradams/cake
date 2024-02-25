//en.cppreference.com/w/c/numeric/math/asinh.html
#include <stdio.h>
#include <math.h>
 
int main(void)
{
    printf("asinh(1) = %f\nasinh(-1) = %f\n", asinh(1), asinh(-1));
    // special values
    printf("asinh(+0) = %f\nasinh(-0) = %f\n", asinh(0.0), asinh(-0.0));
}