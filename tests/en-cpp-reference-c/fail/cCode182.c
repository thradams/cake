//en.cppreference.com/w/c/numeric/complex/cexp.html
#include <stdio.h>
#include <math.h>
#include <complex.h>
 
int main(void)
{
    double PI = acos(-1);
    double complex z = cexp(I * PI); // Euler's formula
    printf("exp(i*pi) = %.1f%+.1fi\n", creal(z), cimag(z));
 
}