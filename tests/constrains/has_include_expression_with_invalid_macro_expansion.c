// Test 7: has_include expression with invalid macro expansion


#define HEADER <stdio.h>

#if __has_include(HEADER) // Error: Macro expansion is not allowed in has_include
    #define HEADER_FOUND 1
#endif

int main() {
    return 0;
}
