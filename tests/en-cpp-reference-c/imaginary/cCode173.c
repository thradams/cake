//en.cppreference.com/w/c/numeric/complex/cacos.html
#include <stdio.h>
#include <math.h>
#include <complex.h>
 
int main(void)
{
    double complex z = cacos(-2);
    printf("cacos(-2+0i) = %f%+fi\n", creal(z), cimag(z));
 
    double complex z2 = cacos(conj(-2)); // or CMPLX(-2, -0.0)
    printf("cacos(-2-0i) (the other side of the cut) = %f%+fi\n", creal(z2), cimag(z2));
 
    // for any z, acos(z) = pi - acos(-z)
    double pi = acos(-1);
    double complex z3 = ccos(pi-z2);
    printf("ccos(pi - cacos(-2-0i) = %f%+fi\n", creal(z3), cimag(z3));
}