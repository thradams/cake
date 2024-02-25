//en.cppreference.com/w/c/numeric/complex/conj.html
#include <stdio.h>
#include <complex.h>
 
int main(void)
{
    double complex z = 1.0 + 2.0*I;
    double complex z2 = conj(z);
    printf("The conjugate of %.1f%+.1fi is %.1f%+.1fi\n",
            creal(z), cimag(z), creal(z2), cimag(z2));
 
    printf("Their product is %.1f%+.1fi\n", creal(z*z2), cimag(z*z2));
}