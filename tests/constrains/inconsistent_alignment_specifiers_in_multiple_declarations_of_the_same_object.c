// Test 8: Inconsistent alignment specifiers in multiple declarations of the same object

alignas(16) int var;

int var; // Error: Inconsistent alignment specifiers for multiple declarations of the same object

int main() {
    return 0;
}
