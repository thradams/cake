//en.cppreference.com/w/c/numeric/complex/complex.html
#include <stdio.h>
#include <complex.h>
#include <math.h>
 
void print_complex(const char* note, complex z)
{
    printf("%s %f%+f*i\n", note, creal(z), cimag(z));
}
 
int main(void)
{
    double complex z = -1.0 + 2.0*I;
    print_complex("z  =", z);
    print_complex("z\u00B2 =", z * z);
    double complex z2 = ccos(2.0 * carg(z)) + csin(2.0 * carg(z))*I;
    print_complex("z\u00B2 =", cabs(z) * cabs(z) * z2);
}