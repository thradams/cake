// Test 3: has_include expression with invalid syntax

#if __has_include(<invalid_path>) // Error: Invalid syntax for has_include expression
    #define HAS_HEADER 1
#endif

int main() {
    return 0;
}
