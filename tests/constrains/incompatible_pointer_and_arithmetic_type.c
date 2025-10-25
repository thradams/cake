// Test 7: Incompatible pointer and arithmetic type


int main() {
    int x = 10;
    int *ptr = &x;
    int result = ptr + x; // Error: Cannot add pointer and integer
    return 0;
}
