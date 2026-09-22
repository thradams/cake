// Test 6: Enum specifier with incompatible underlying types

enum EnumType : int {
    CONST_A = 1
};

enum EnumType : short {
    CONST_B = 2
}; //lint 950 2070 enum redeclared with different underlying type / redefinition with different content

int main() {
    return 0;
}
