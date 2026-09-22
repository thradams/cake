// Test 8: Incompatible initializer for compound literal type

int main() {
    (int){"Hello"}; //lint 75 pointer to integer conversion
    return 0;
}
