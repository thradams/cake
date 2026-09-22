// Test 2: Subtraction of pointer and floating-point number

int main() {
    int arr[10];
    float f = 5.0;
    int *p = arr - f; //lint 850 1340 right must be integer type
    return 0;
}
