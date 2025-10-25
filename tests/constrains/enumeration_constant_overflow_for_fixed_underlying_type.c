// Test 2: Enumeration constant overflow for fixed underlying type


enum OverflowEnum : unsigned char {
    CONST_MAX = 255,
    CONST_OVERFLOW // Error: Value of enumeration constant exceeds maximum for unsigned char
};

int main() {
    return 0;
}
