// Test 2: Enumeration constant overflow for fixed underlying type


enum OverflowEnum : unsigned char {
    CONST_MAX = 255,
    CONST_OVERFLOW
}; //lint 860 enumerator value outside of underlying type range

int main() {
    return 0;
}
