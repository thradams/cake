// Test 3: Implicit conversion from int* to float*

int main() {
    int x = 5;
    int *int_ptr = &x;
    float *float_ptr = int_ptr; // Error: Implicit conversion from int* to float*
    return 0;
}
