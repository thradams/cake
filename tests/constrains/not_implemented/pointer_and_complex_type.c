// Test 6: Pointer and complex type
#include <stdio.h>
#include <complex.h>

int main() {
    int cond = 1;
    int *ptr = NULL;
    double complex z = 1.0 + 2.0 * I;
    void *result = cond ? ptr : z; // Error: Cannot mix pointer and complex type
    return 0;
}
