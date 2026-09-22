// Test 3: Duplicate compatible types

#define GENERIC_EXAMPLE(x) _Generic((x), int: "int", signed int: "signed int", default: "default")

int main() {
    int x = 5;
    GENERIC_EXAMPLE(x); //lint 1570 two compatible types
    return 0;
}
