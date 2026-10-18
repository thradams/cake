// Test 10: Enumeration with underlying type specified as an enumeration type

enum BaseEnum {
    BASE_A = 1,
    BASE_B = 2
};

enum EnumUnderlying : BaseEnum { // Error: Enumeration cannot have another enum as underlying type
    CONST_A = 3,
    CONST_B = 4
};

int main() {
    return 0;
}
