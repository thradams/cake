// Test 4: Invalid initializer list for array compound literal


int main() {
    (int[3]){1}; // Error: Incomplete initializer list for array of size 3
    return 0;
}
