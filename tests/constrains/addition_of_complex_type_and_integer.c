// Test 7: Addition of complex type and integer
#include <stdio.h>
#include <complex.h>

int main() {
    double complex z = 1.0 + 2.0 * I;
    int i = 5;
    double complex result = z + i; // Error: Cannot add integer to complex type
    return 0;
}
