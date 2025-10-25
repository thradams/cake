// Test 2: Using . operator with an invalid member
#include <stdio.h>

struct Point {
    int x, y;
};

int main() {
    struct Point p = {10, 20};
    p.z = 30; // Error: z is not a member of struct Point
    return 0;
}
