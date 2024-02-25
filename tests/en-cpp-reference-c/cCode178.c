//en.cppreference.com/w/c/numeric/complex/catan.html
#include <stdio.h>
#include <float.h>
#include <complex.h>
 
int main(void)
{
    double complex z = catan(2*I);
    printf("catan(+0+2i) = %f%+fi\n", creal(z), cimag(z));
 
    double complex z2 = catan(-conj(2*I)); // or CMPLX(-0.0, 2)
    printf("catan(-0+2i) (the other side of the cut) = %f%+fi\n", creal(z2), cimag(z2));
 
    double complex z3 = 2*catan(2*I*DBL_MAX); // or CMPLX(0, INFINITY)
    printf("2*catan(+0+i*Inf) = %f%+fi\n", creal(z3), cimag(z3));
}