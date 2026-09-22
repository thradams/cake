// Test 2: Applying & to a register variable

int main() {
    register int x = 5;
    int *p = &x; //lint 1220  // Error: Cannot take the address of a register variable
    return 0;
}
