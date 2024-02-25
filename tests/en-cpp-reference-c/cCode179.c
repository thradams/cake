//en.cppreference.com/w/c/numeric/complex/catanh.html
#include <stdio.h>
#include <complex.h>
 
int main(void)
{
    double complex z = catanh(2);
    printf("catanh(+2+0i) = %f%+fi\n", creal(z), cimag(z));
 
    double complex z2 = catanh(conj(2)); // or catanh(CMPLX(2, -0.0)) in C11
    printf("catanh(+2-0i) (the other side of the cut) = %f%+fi\n", creal(z2), cimag(z2));
 
    // for any z, atanh(z) = atan(iz)/i
    double complex z3 = catanh(1+2*I);
    printf("catanh(1+2i) = %f%+fi\n", creal(z3), cimag(z3));
    double complex z4 = catan((1+2*I)*I)/I;
    printf("catan(i * (1+2i))/i = %f%+fi\n", creal(z4), cimag(z4));
}