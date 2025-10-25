// Test 5: Inline function with external linkage not defined in the same translation unit


inline void inline_func(); // Error: Inline function with external linkage must be defined in the same translation unit

int main() {
    inline_func();
    return 0;
}
