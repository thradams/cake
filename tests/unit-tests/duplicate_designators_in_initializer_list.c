// Test 6: Duplicate designators in initializer list

struct Point {
    int x, y;
};

int main() {
    (struct Point){.x = 5, .x = 10}; // Error: Duplicate designator 'x'
    return 0;
}
