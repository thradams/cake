// Test 5: Addition of a pointer and a floating-point number

int main() {
    int arr[10];
    double d = 2.5;
    int *p = arr + d; // Error: Cannot add a double to a pointer
    return 0;
}
