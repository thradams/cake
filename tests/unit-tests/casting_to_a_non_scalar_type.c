// Test 1: Casting to a non-scalar type


struct Point {
    int x, y;
};

int main() {
    int x = 5;
    (struct Point)x; //lint 2260
    return 0;
}
