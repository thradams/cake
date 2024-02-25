//en.cppreference.com/w/c/numeric/tgmath.html
#include <stdio.h>
#include <tgmath.h>
 
int main(void)
{
    int i = 2;
    printf("sqrt(2) = %f\n", sqrt(i)); // argument type is int, calls sqrt
 
    float f = 0.5;
    printf("sin(0.5f) = %f\n", sin(f));   // argument type is float, calls sinf
 
    float complex dc = 1 + 0.5*I;
    float complex z = sqrt(dc);      // argument type is float complex, calls csqrtf
    printf("sqrt(1 + 0.5i) = %f+%fi\n",
           creal(z),  // argument type is float complex, calls crealf
           cimag(z)); // argument type is float complex, calls cimagf
}