// Test 9: Use of __VA_OPT__ without ellipsis in macro definition
#include <stdio.h>

#define SHOW(msg) printf(msg __VA_OPT__(, "extra")) // Error: __VA_OPT__ used in non-variadic macro

int main() {
    SHOW("Message");
    return 0;
}
