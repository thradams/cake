//en.cppreference.com/w/c/numeric/complex/clog.html
#include <stdio.h>
#include <math.h>
#include <complex.h>
 
int main(void)
{
    double complex z = clog(I); // r = 1, θ = pi/2
    printf("2*log(i) = %.1f%+fi\n", creal(2*z), cimag(2*z));
 
    double complex z2 = clog(sqrt(2)/2 + sqrt(2)/2*I); // r = 1, θ = pi/4
    printf("4*log(sqrt(2)/2+sqrt(2)i/2) = %.1f%+fi\n", creal(4*z2), cimag(4*z2));
 
    double complex z3 = clog(-1); // r = 1, θ = pi
    printf("log(-1+0i) = %.1f%+fi\n", creal(z3), cimag(z3));
 
    double complex z4 = clog(conj(-1)); // or clog(CMPLX(-1, -0.0)) in C11
    printf("log(-1-0i) (the other side of the cut) = %.1f%+fi\n", creal(z4), cimag(z4));
}