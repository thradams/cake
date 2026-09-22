// Test 8: Redefining enum with different members

enum Color {
    RED, GREEN, BLUE
};

enum Color {
    RED, YELLOW, BLUE
}; //lint 2070 redefinition of enum with different content

int main() {
    return 0;
}
