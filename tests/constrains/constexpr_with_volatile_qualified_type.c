// Test 7: constexpr with volatile qualified type


constexpr volatile int b = 20; // Error: constexpr cannot be volatile-qualified

int main() {
    return 0;
}
