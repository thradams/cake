// Test 7: Enum specifier with bit-precise integer type as underlying type


enum BitEnum : _BitInt(16) { //lint 2220
    CONST_A = 1,
    CONST_B = 2
};

int main() {
    return 0;
}
