// Test 1: Enumeration constant value out of range for fixed underlying type

enum LargeEnum : unsigned char {
    CONST_A = 256
}; //lint 860 enumerator value outside of underlying type range

int main() {
    return 0;
}
