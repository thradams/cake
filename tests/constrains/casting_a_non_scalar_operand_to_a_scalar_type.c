// Test 2: Casting a non-scalar operand to a scalar type

struct Point {
    int x, y;
};

int main() {
    struct Point p = {10, 20};
    (int)p; // Error: Operand of cast must be scalar
    return 0;
}
