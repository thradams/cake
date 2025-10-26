// Test 3: Using -> operator on a non-pointer type

struct Point {
    int x, y;
};

int main() {
    struct Point p = {10, 20};
    p->x = 30; // Error: p is not a pointer to struct
    return 0;
}
