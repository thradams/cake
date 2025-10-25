// Test 5: Applying & to a const literal

int main() {
    const int x = 10;
    int *p = &x; // Error: Taking the address of a const literal
    return 0;
}
