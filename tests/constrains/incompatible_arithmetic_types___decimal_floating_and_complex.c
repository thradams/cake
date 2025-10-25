// Test 2: Incompatible arithmetic types - decimal floating and complex
#include <stdio.h>
#include <complex.h>
#include <decimal.h>

int main() {
    _Decimal64 d = 1.0dd;
    double complex z = 1.0 + 2.0 * I;
    _Decimal64 result = d + z; // Error: Cannot combine decimal floating and complex types
    return 0;
}
