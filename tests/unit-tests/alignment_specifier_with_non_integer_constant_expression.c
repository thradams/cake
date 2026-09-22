// Test 5: Alignment specifier with non-integer constant expression

alignas(8.0) int x; //lint 2160 alignment specifier must be an integer constant expression

int main() {
    return 0;
}
