//en.cppreference.com/w/c/numeric/math/tanh.html
#include <stdio.h>
#include <math.h>
 
int main(void)
{
    printf("tanh(1) = %f\ntanh(-1) = %f\n", tanh(1), tanh(-1));
    printf("tanh(0.1)*sinh(0.2)-cosh(0.2) = %f\n", tanh(0.1) * sinh(0.2) - cosh(0.2));
    // special values
    printf("tanh(+0) = %f\ntanh(-0) = %f\n", tanh(0.0), tanh(-0.0));
}