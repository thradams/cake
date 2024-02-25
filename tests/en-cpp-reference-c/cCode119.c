//en.cppreference.com/w/c/language/operator_arithmetic.html
#include <stdio.h>
#include <complex.h>
#include <limits.h>
 
int main(void)
{
    char c = 'a';
    printf("sizeof char: %zu sizeof int: %zu\n", sizeof c, sizeof +c);
 
    printf("-1, where 1 is signed: %d\n", -1);
 
    // Defined behavior since arithmetic is performed for unsigned integer.
    // Hence, the calculation is (-1) modulo (2 raised to n) = UINT_MAX, where n is
    // the number of bits of unsigned int. If unsigned int is 32-bit long, then this
    // gives (-1) modulo (2 raised to 32) = 4294967295
    printf("-1, where 1 is unsigned: %u\n", -1u); 
 
    // Undefined behavior because the mathematical value of -INT_MIN = INT_MAX + 1
    // (i.e. 1 more than the maximum possible value for signed int)
    //
    // printf("%d\n", -INT_MIN);
 
    // Undefined behavior because the mathematical value of -LONG_MIN = LONG_MAX + 1
    // (i.e. 1 more than the maximum possible value for signed long)
    //
    // printf("%ld\n", -LONG_MIN);
 
    // Undefined behavior because the mathematical value of -LLONG_MIN = LLONG_MAX + 1
    // (i.e. 1 more than the maximum possible value for signed long long)
    //
    // printf("%lld\n", -LLONG_MIN);
 
    double complex z = 1 + 2*I;
    printf("-(1+2i) = %.1f%+.1f\n", creal(-z), cimag(-z));
}