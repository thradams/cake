// Test 8: Implicit conversion from NULL to function pointer

void my_function() {}

int main() {
    void (*func_ptr)() = NULL; // Error: Implicit conversion from NULL to function pointer
    return 0;
}
