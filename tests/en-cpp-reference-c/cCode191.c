//en.cppreference.com/w/c/numeric/complex/csinh.html
#include <stdio.h>
#include <math.h>
#include <complex.h>
 
int main(void)
{
    double complex z = csinh(1);  // behaves like real sinh along the real line
    printf("sinh(1+0i) = %f%+fi (sinh(1)=%f)\n", creal(z), cimag(z), sinh(1));
 
    double complex z2 = csinh(I); // behaves like sine along the imaginary line
    printf("sinh(0+1i) = %f%+fi ( sin(1)=%f)\n", creal(z2), cimag(z2), sin(1));
}