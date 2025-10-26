// Test 5: Use of __VA_ARGS__ in non-variadic macro

void printf(const char* s);
#define PRINT(args) printf(args, __VA_ARGS__) // Error: __VA_ARGS__ used in non-variadic macro

int main() {
    PRINT("Hello, world!\n");
    return 0;
}
