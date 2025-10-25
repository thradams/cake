// Test 3: Addition of decimal floating-point and standard floating-point
#include <stdio.h>
#include <decimal.h>

int main() {
    _Decimal32 d = 1.0df;
    float f = 2.0f;
    _Decimal32 result = d + f; // Error: Cannot add decimal floating-point and standard floating-point
    return 0;
}
