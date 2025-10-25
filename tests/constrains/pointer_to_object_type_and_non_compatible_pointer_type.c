// Test 4: Pointer to object type and non-compatible pointer type

int main() {
    int x = 10;
    int *int_ptr = &x;
    double *double_ptr = int_ptr; // Error: Incompatible pointer types
    return 0;
}
