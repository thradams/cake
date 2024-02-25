//en.cppreference.com/w/c/language/arithmetic_types.html
#include <complex.h>
#include <math.h>
#include <stdio.h>
 
int main(void)
{
    double complex z = (1 + 0*I) * (INFINITY + I*INFINITY);
//  textbook formula would give
//  (1+i0)(∞+i∞) ⇒ (1×∞ – 0×∞) + i(0×∞+1×∞) ⇒ NaN + I*NaN
//  but C gives a complex infinity
    printf("%f%+f*i\n", creal(z), cimag(z));
 
//  textbook formula would give
//  cexp(∞+iNaN) ⇒ exp(∞)×(cis(NaN)) ⇒ NaN + I*NaN
//  but C gives  ±∞+i*nan
    double complex y = cexp(INFINITY + I*NAN);
    printf("%f%+f*i\n", creal(y), cimag(y));
}