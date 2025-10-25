// Test 6: Enum specifier with incompatible underlying types

enum EnumType : int {
    CONST_A = 1
};

enum EnumType : short { // Error: Incompatible underlying type for EnumType
    CONST_B = 2
};

int main() {
    return 0;
}
