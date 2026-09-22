// Test 3: Non-integer constant expression in enumeration without fixed underlying type

enum NonIntegerEnum {
    CONST_A = 3.14
}; //lint 860

int main() {
    return 0;
}
