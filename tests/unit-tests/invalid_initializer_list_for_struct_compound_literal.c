// Test 3: Invalid initializer list for struct compound literal


struct Point {
    int x;
    int y;
};

int main() {
    (struct Point){1, 2, 3}; //lint 62 excess elements in initializer
    return 0;
}
