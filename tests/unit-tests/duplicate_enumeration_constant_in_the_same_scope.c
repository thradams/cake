// Test 8: Duplicate enumeration constant in the same scope

enum Color {
    RED,
    GREEN,
    BLUE,
    RED
}; //lint 2210

int main() {
    return 0;
}
