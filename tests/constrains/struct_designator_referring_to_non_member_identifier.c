// Test 8: Struct designator referring to non-member identifier

struct Point {
    int x;
    int y;
};

struct Point p = {.z = 10}; // Error: Designator .z does not match any member of struct Point

int main() {
    return 0;
}
