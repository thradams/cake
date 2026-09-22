// Test 5: Controlling type compatible with more than one type

#define GENERIC_EXAMPLE(x) _Generic((x), double: "double", double*: "double pointer", default: "default")

int main() {
    double x = 5.0;
    GENERIC_EXAMPLE(&x);
    return 0;
}
