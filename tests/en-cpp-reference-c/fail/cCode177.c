//en.cppreference.com/w/c/numeric/complex/casinh.html
#include <stdio.h>
#include <complex.h>
 
int main(void)
{
    double complex z = casinh(0+2*I);
    printf("casinh(+0+2i) = %f%+fi\n", creal(z), cimag(z));
 
    double complex z2 = casinh(-conj(2*I)); // or casinh(CMPLX(-0.0, 2)) in C11
    printf("casinh(-0+2i) (the other side of the cut) = %f%+fi\n", creal(z2), cimag(z2));
 
    // for any z, asinh(z) = asin(iz)/i
    double complex z3 = casinh(1+2*I);
    printf("casinh(1+2i) = %f%+fi\n", creal(z3), cimag(z3));
    double complex z4 = casin((1+2*I)*I)/I;
    printf("casin(i * (1+2i))/i = %f%+fi\n", creal(z4), cimag(z4));
}