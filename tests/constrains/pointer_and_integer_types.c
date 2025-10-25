// Test 7: Pointer and integer types


int main() {
    int cond = 1;
    int *ptr = NULL;
    int x = 5;
    void *result = cond ? ptr : x; // Error: Cannot mix pointer and integer types
    return 0;
}
