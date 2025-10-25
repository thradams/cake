// Test 1: Incompatible arithmetic types - decimal and standard floating-point



int main() {
    _Decimal32 d = 1.0df;
    float f = 2.0f;
    _Decimal32 result = d + f; // Error: Cannot combine decimal and standard floating-point types
    return 0;
}
