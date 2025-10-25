// Test 5: Incompatible nullptr_t and function pointer type

int main() {
    
    void (*func_ptr)() = nullptr; // Error: Cannot assign nullptr_t to function pointer
    return 0;
}
