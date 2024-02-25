//en.cppreference.com/w/c/numeric/complex/ctan.html
#include <stdio.h>
#include <math.h>
#include <complex.h>
 
int main(void)
{
    double complex z = ctan(1);  // behaves like real tangent along the real line
    printf("tan(1+0i) = %f%+fi ( tan(1)=%f)\n", creal(z), cimag(z), tan(1));
 
    double complex z2 = ctan(I); // behaves like tanh along the imaginary line 
    printf("tan(0+1i) = %f%+fi (tanh(1)=%f)\n", creal(z2), cimag(z2), tanh(1));
}