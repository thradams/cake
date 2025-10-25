// Test 8: Incompatible initializer for compound literal type

int main() {
    (int){"Hello"}; // Error: Incompatible initializer for int type
    return 0;
}
