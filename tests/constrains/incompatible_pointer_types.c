// Test 4: Incompatible pointer types

int main() {
    int cond = 1;
    int *p1 = 0;
    float *p2 = 0;
    void *result = cond ? p1 : p2; // Error: Incompatible pointer types
    return 0;
}
