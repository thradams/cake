// Test 1: Multiple default generic associations

#define GENERIC_EXAMPLE(x) _Generic((x), int: "int", default: "default", default: "another default")

int main() {
    int x = 5;
    GENERIC_EXAMPLE(x); //lint 1570 duplicate default generic association
    return 0;
}
