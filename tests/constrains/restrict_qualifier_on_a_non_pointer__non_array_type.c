// Test 1: restrict qualifier on a non-pointer, non-array type

int restrict x = 10; // Error: restrict qualifier cannot be applied to non-pointer, non-array types

int main() {
    return 0;
}
