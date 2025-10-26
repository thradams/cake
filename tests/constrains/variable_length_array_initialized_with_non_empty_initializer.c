// Test 3: Variable-length array initialized with non-empty initializer

void func(int n) {
    int arr[n] = {1, 2, 3}; // Error: Variable-length array cannot be initialized with values
}
