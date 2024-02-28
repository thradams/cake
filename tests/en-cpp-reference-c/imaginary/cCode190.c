//en.cppreference.com/w/c/numeric/complex/csin.html
#include <stdio.h>
#include <math.h>
#include <complex.h>
 
int main(void)
{
    double complex z = csin(1);  // behaves like real sine along the real line
    printf("sin(1+0i) = %f%+fi ( sin(1)=%f)\n", creal(z), cimag(z), sin(1));
 
    double complex z2 = csin(I); // behaves like sinh along the imaginary line 
    printf("sin(0+1i) = %f%+fi (sinh(1)=%f)\n", creal(z2), cimag(z2), sinh(1));
}