// Test 11: constexpr with out-of-range floating-point initializer

#include <float.h>

constexpr float e = FLT_MAX * 10.0f; // Error: out-of-range value for constexpr float initializer

int main() {
    return 0;
}
