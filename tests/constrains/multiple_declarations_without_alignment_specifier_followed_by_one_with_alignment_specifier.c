// Test 9: Multiple declarations without alignment specifier followed by one with alignment specifier

int var;
alignas(32) int var; // Error: Declaration with alignment specifier conflicts with previous declaration without one

int main() {
    return 0;
}
