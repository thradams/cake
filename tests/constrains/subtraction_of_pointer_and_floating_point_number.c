// Test 2: Subtraction of pointer and floating-point number

int main() {
    int arr[10];
    float f = 5.0;
    int *p = arr - f; // Error: Cannot subtract a float from a pointer
    return 0;
}
