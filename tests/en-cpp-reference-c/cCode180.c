//en.cppreference.com/w/c/numeric/complex/ccos.html
#include <stdio.h>
#include <math.h>
#include <complex.h>
 
int main(void)
{
    double complex z = ccos(1);  // behaves like real cosine along the real line
    printf("cos(1+0i) = %f%+fi ( cos(1)=%f)\n", creal(z), cimag(z), cos(1));
 
    double complex z2 = ccos(I); // behaves like real cosh along the imaginary line
    printf("cos(0+1i) = %f%+fi (cosh(1)=%f)\n", creal(z2), cimag(z2), cosh(1));
}