// Test 1: Casting to a non-scalar type


struct Point {
    int x, y;
};

int main() {
    int x = 5;
    (struct Point)x; // Error: Cannot cast to a non-scalar type
    return 0;
}
