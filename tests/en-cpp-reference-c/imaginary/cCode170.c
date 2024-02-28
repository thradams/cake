//en.cppreference.com/w/c/numeric/complex/I.html
#include <stdio.h>
#include <complex.h>
 
int main(void)
{
    printf("I = %.1f%+.1fi\n", creal(I), cimag(I));
 
    double complex z1 = I * I;     // imaginary unit squared
    printf("I * I = %.1f%+.1fi\n", creal(z1), cimag(z1));
 
    double complex z = 1.0 + 2.0*I; // usual way to form a complex number pre-C11
    printf("z = %.1f%+.1fi\n", creal(z), cimag(z));
}