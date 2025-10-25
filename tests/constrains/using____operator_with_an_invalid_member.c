// Test 4: Using -> operator with an invalid member
#include <stdio.h>

struct Point {
    int x, y;
};

int main() {
    struct Point *p = NULL;
    p->z = 30; // Error: z is not a member of struct Point
    return 0;
}
