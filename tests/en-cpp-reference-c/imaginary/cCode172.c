//en.cppreference.com/w/c/numeric/complex/cabs.html
#include <stdio.h>
#include <complex.h>
 
int main(void)
{
    double complex z = 1.0 + 1.0*I;
    printf("%.1f%+.1fi cartesian is rho=%f theta=%f polar\n",
           creal(z), cimag(z), cabs(z), carg(z));
}