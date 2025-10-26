// Test 7: Pointer and integer types


int main() {
    int cond = 1;
    int *ptr = 0;
    int x = 5;
    void *result = cond ? ptr : x; // Error: Cannot mix pointer and integer types
    return 0;
}
