// Test 6: Applying & to an array without indexing


int main() {
    int arr[5] = {1, 2, 3, 4, 5};
    int *p = &arr; // Error: &arr is not an array element
    return 0;
}
