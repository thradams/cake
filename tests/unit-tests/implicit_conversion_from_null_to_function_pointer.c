// Test 8: Implicit conversion from NULL to function pointer

#include <stddef.h>

void my_function() {}

int main() {
    void (*func_ptr)() = NULL;
    return 0;
}
