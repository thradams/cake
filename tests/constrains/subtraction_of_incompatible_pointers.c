// Test 4: Subtraction of incompatible pointers

int main() {
    int a = 10;
    float b = 20.0;
    int *p1 = &a;
    float *p2 = &b;
    int diff = p1 - p2; // Error: Cannot subtract incompatible pointer types
    return 0;
}
