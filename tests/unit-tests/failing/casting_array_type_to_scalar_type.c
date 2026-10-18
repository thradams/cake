// Test 8: Casting array type to scalar type

int main() {
    int arr[3] = {1, 2, 3};
    int x = (int)arr; // Error: Cannot cast array to scalar type
    return 0;
}
