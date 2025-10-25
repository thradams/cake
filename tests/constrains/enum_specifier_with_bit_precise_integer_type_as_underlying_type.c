// Test 7: Enum specifier with bit-precise integer type as underlying type


enum BitEnum : _BitInt(16) { // Error: Bit-precise integer type not allowed as enum underlying type
    CONST_A = 1,
    CONST_B = 2
};

int main() {
    return 0;
}
