// Test 6: Constant expression with out-of-range value

#include <limits.h>

const int value = INT_MAX + 1; // Error: Value is out of range for int type

int main() {
    return 0;
}
