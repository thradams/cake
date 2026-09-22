// Test 2: Compound literal with variable-length array

int main() {
    int n = 5;
    (int[n]){1, 2, 3, 4, 5}; //lint 740
    return 0;
}
