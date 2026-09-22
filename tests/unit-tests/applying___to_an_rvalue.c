// Test 3: Applying & to an rvalue


int main() {
    int *p = &(5 + 3); //lint 1220  // Error: Cannot take the address of an rvalue
    return 0;
}
