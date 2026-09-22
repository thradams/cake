// Test 3: Implicit conversion from int* to float*

int main() {
    int x = 5;
    int *int_ptr = &x;
    float *float_ptr = int_ptr; //lint 54 incompatible types
    return 0;
}
