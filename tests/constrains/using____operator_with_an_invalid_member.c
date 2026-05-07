// Test 4: Using -> operator with an invalid member

struct Point {
    int x, y;
};

int main() {
    struct Point *p = 0;
    
    p->z = 30; //lint 720 Error: z is not a member of struct Point
    return 0;
}
