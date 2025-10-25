// Test 4: Applying & to a cast expression


int main() {
    int x = 5;
    int *p = &(int)x; // Error: Cannot take the address of a cast expression
    return 0;
}
