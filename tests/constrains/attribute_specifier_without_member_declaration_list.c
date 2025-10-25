// Test 9: Attribute specifier without member declaration list

struct __attribute__((aligned(16))) TestStruct; // Error: Attribute specifier without member declaration list

int main() {
    return 0;
}
