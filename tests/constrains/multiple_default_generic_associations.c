// Test 1: Multiple default generic associations

#define GENERIC_EXAMPLE(x) _Generic((x), int: "int", default: "default", default: "another default")

int main() {
    int x = 5;
    GENERIC_EXAMPLE(x); // Should flag error due to multiple defaults
    return 0;
}
