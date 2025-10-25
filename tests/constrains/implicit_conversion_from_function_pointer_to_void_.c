// Test 6: Implicit conversion from function pointer to void*

void my_function() {}

int main() {
    void *ptr = my_function; // Error: Implicit conversion from function pointer to void*
    return 0;
}
