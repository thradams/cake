// Test 6: Incompatible pointer and complex type
#include <stdio.h>
#include <complex.h>

int main() {
    int x = 5;
    int *ptr = &x;
    double complex z = 1.0 + 2.0 * I;
    double complex result = ptr + z; // Error: Cannot add pointer and complex type
    return 0;
}
