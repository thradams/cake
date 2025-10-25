// Test 2: Compound literal with variable-length array

int main() {
    int n = 5;
    (int[n]){1, 2, 3, 4, 5}; // Error: VLA type is not allowed for compound literals
    return 0;
}
