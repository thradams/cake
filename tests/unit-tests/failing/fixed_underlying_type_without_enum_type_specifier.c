// Test 5: Fixed underlying type without enum type specifier


enum MyEnum {
    CONST_A = 1,
    CONST_B = 2
} : int; // Error: Fixed underlying type specified without enum type specifier

int main() {
    return 0;
}
