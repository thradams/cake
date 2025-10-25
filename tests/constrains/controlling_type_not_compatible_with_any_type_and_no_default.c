// Test 4: Controlling type not compatible with any type and no default


#define GENERIC_EXAMPLE(x) _Generic((x), int: "int", double: "double")

int main() {
    float x = 5.0f;
    GENERIC_EXAMPLE(x); // Should flag error due to no matching type and no default
    return 0;
}
