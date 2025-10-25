// Test 8: Duplicate enumeration constant in the same scope

enum Color {
    RED,
    GREEN,
    BLUE,
    RED // Error: Duplicate enumeration constant in the same scope
};

int main() {
    return 0;
}
