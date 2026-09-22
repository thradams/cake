// Test 2: Implicit conversion from void* to int*

int main() {
    void *ptr = 0;
    int *int_ptr = ptr; // Error: Implicit conversion from void* to int*
    
    return 0;
}
