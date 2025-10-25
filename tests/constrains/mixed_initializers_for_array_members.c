// Test 10: Mixed initializers for array members

int arr[3] = {1, [2] = 5, 3}; // Error: Mixing designated and non-designated initializers

int main() {
    return 0;
}
