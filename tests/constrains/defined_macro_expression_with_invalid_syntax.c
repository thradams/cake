// Test 6: defined macro expression with invalid syntax

#if defined(123) // Error: Invalid syntax for defined operator
    #define MACRO_DEFINED 1
#endif

int main() {
    return 0;
}
