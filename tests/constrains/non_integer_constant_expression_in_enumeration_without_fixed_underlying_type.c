// Test 3: Non-integer constant expression in enumeration without fixed underlying type

enum NonIntegerEnum {
    CONST_A = 3.14 // Error: Enumeration constant must be an integer constant expression
};

int main() {
    return 0;
}
