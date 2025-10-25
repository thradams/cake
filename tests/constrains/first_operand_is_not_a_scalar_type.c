// Test 1: First operand is not a scalar type


struct Point {
    int x, y;
};

int main() {
    struct Point p = {1, 2};
    int result = p ? 1 : 0; // Error: First operand is not a scalar type
    return 0;
}
