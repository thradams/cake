// Test 2: Incompatible arithmetic types (decimal and standard floating)
#include <stdio.h>
#include <decimal.h>

int main() {
    int cond = 1;
    _Decimal32 d = 1.0df;
    float f = 2.0f;
    _Decimal32 result = cond ? d : f; // Error: Cannot combine decimal and standard floating types
    return 0;
}
