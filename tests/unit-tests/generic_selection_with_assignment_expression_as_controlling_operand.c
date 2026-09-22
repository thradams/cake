// Test 6: Generic selection with assignment expression as controlling operand


#define GENERIC_EXAMPLE(x) _Generic((x), int *: "int pointer", default: "default")

int main() {
    int arr[5] = {1, 2, 3, 4, 5};
    GENERIC_EXAMPLE(arr = arr); //lint 910 assignment to expression with array type
    return 0;
}
