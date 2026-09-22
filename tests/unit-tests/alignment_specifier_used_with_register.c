// Test 2: Alignment specifier used with register

void func() {
    register alignas(16) int x = 0; //lint 2140 alignment specifier cannot be used with 'register'
}
