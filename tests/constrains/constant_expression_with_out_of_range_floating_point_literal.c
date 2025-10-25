// Test 7: Constant expression with out-of-range floating-point literal
#include <float.h>

const float value = FLT_MAX * 10.0f; // Error: Value is out of range for float type

int main() {
    return 0;
}
