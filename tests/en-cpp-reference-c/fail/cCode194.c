//en.cppreference.com/w/c/numeric/complex/ctanh.html
#include <stdio.h>
#include <math.h>
#include <complex.h>
 
int main(void)
{
    double complex z = ctanh(1);  // behaves like real tanh along the real line
    printf("tanh(1+0i) = %f%+fi (tanh(1)=%f)\n", creal(z), cimag(z), tanh(1));
 
    double complex z2 = ctanh(I); // behaves like tangent along the imaginary line
    printf("tanh(0+1i) = %f%+fi ( tan(1)=%f)\n", creal(z2), cimag(z2), tan(1));
}