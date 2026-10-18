// Test 9: Enumeration without fixed underlying type including an enum type specifier


enum NonFixedEnum : int { // Error: Enumeration without fixed underlying type cannot include type specifier
    CONST_A = 1,
    CONST_B = 2
};

int main() {
    return 0;
}
