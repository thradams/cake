//en.cppreference.com/w/c/numeric/complex/cpow.html
#include <stdio.h>
#include <complex.h>
 
int main(void)
{    
    double complex z = cpow(1.0+2.0*I, 2);
    printf("(1+2i)^2 = %.1f%+.1fi\n", creal(z), cimag(z));
 
    double complex z2 = cpow(-1, 0.5);
    printf("(-1+0i)^0.5 = %.1f%+.1fi\n", creal(z2), cimag(z2));
 
    double complex z3 = cpow(conj(-1), 0.5); // other side of the cut
    printf("(-1-0i)^0.5 = %.1f%+.1fi\n", creal(z3), cimag(z3));
 
    double complex z4 = cpow(I, I); // i^i = exp(-pi/2)
    printf("i^i = %f%+fi\n", creal(z4), cimag(z4));
}