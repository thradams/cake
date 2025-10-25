// Test 8: Redefining enum with different members

enum Color {
    RED, GREEN, BLUE
};

enum Color {
    RED, YELLOW, BLUE // Error: Enum members do not match previous declaration
};

int main() {
    return 0;
}
