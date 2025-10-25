// Test 1: Alignment specifier used with typedef

typedef alignas(16) int aligned_typedef; // Error: Alignment specifier cannot be used with typedef

int main() {
    return 0;
}
