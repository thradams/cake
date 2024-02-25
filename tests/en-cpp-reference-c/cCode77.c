//en.cppreference.com/w/c/language/arithmetic_types.html
#include <complex.h>
#include <stdio.h>
 
int main(void)
{
    double complex z = 1 + 2*I;
    z = 1 / z;
    printf("1/(1.0+2.0i) = %.1f%+.1fi\n", creal(z), cimag(z));
}