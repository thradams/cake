// Test 5: Using . operator on a pointer to structure

struct Point {
    int x, y;
};

int main() {
    struct Point p = {10, 20};
    struct Point *ptr = &p;
    ptr.x = 15; // Error: ptr is a pointer, not a structure or union
    return 0;
}
