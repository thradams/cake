//en.cppreference.com/w/c/numeric/complex/CMPLX.html
#include <stdio.h>
#include <complex.h>
 
int main(void)
{
    double complex z = CMPLX(0.0, -0.0);
    printf("z = %.1f%+.1fi\n", creal(z), cimag(z));
}