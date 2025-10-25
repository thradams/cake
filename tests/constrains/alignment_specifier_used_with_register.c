// Test 2: Alignment specifier used with register

void func() {
    register alignas(16) int x = 0; // Error: Alignment specifier cannot be used with register
}
