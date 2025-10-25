// Test 7: Using . operator with undeclared member in qualified structure
#include <stdio.h>

struct Point {
    const int x;
};

int main() {
    struct Point p = {10};
    p.y = 20; // Error: y is not a member of struct Point
    return 0;
}
