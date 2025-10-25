// Test 3: Duplicate compatible types

#define GENERIC_EXAMPLE(x) _Generic((x), int: "int", signed int: "signed int", default: "default")

int main() {
    int x = 5;
    GENERIC_EXAMPLE(x); // Should flag error due to duplicate compatible types
    return 0;
}
