//en.cppreference.com/w/c/numeric/complex/cacosh.html
#include <stdio.h>
#include <complex.h>
 
int main(void)
{
    double complex z = cacosh(0.5);
    printf("cacosh(+0.5+0i) = %f%+fi\n", creal(z), cimag(z));
 
    double complex z2 = conj(0.5); // or cacosh(CMPLX(0.5, -0.0)) in C11
    printf("cacosh(+0.5-0i) (the other side of the cut) = %f%+fi\n", creal(z2), cimag(z2));
 
    // in upper half-plane, acosh(z) = i*acos(z) 
    double complex z3 = casinh(1+I);
    printf("casinh(1+1i) = %f%+fi\n", creal(z3), cimag(z3));
    double complex z4 = I*casin(1+I);
    printf("I*asin(1+1i) = %f%+fi\n", creal(z4), cimag(z4));
}