//en.cppreference.com/w/c/language/arithmetic_types.html
#include <complex.h>
#include <stdio.h>
 
int main(void)
{
    double imaginary z = 3*I;
    z = 1 / z;
    printf("1/(3.0i) = %+.1fi\n", cimag(z));
}