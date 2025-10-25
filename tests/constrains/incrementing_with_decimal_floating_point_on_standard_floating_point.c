// Test 6: Incrementing with decimal floating-point on standard floating-point


int main() {
    float f = 2.0f;
    _Decimal32 d = 1.0df;
    f += d; // Error: Cannot increment standard floating-point by decimal floating-point type
    return 0;
}
