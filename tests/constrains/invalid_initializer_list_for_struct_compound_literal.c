// Test 3: Invalid initializer list for struct compound literal


struct Point {
    int x;
    int y;
};

int main() {
    (struct Point){1, 2, 3}; // Error: Too many initializers for struct Point
    return 0;
}
