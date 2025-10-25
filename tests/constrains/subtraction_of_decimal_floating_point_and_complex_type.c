// Test 8: Subtraction of decimal floating-point and complex type
#include <stdio.h>
#include <complex.h>
#include <decimal.h>

int main() {
    _Decimal32 d = 2.0df;
    double complex z = 1.0 + 2.0 * I;
    _Decimal32 result = d - z; // Error: Cannot subtract complex type from decimal floating-point
    return 0;
}
