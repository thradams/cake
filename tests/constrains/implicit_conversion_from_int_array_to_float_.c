// Test 5: Implicit conversion from int array to float*

int main() {
    int arr[5] = {1, 2, 3, 4, 5};
    float *float_ptr = arr; // Error: Implicit conversion from int[] to float*
    return 0;
}
