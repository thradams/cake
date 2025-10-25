// Test 10: Function-like macro with fewer arguments than required by ellipsis

#define LOG(level, ...) printf(level __VA_OPT__(, __VA_ARGS__))

int main() {
    LOG(); // Error: Macro invocation does not satisfy parameter requirements for variadic macro
    return 0;
}
