// Test 9: Multiple declarations without alignment specifier followed by one with alignment specifier

int var;
alignas(32) int var; //lint 2310

int main() {
    return 0;
}
