// Test 2: Undefined identifier in conditional inclusion
#include <stdio.h>

#if UNDEFINED_IDENTIFIER // Error: Undefined identifier used without defined()
    #define TEST 1
#endif

int main() {
    return 0;
}
