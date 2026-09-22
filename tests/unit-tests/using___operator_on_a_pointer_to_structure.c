// Test 5: Using . operator on a pointer to structure

struct Point {
    int x, y;
};

int main() {
    struct Point p = {10, 20};
    struct Point *ptr = &p;
    ptr.x = 15; //lint 730 structure or union required
    return 0;
}
