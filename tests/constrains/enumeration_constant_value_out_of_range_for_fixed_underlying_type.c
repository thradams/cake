// Test 1: Enumeration constant value out of range for fixed underlying type

enum LargeEnum : unsigned char {
    CONST_A = 256 // Error: Value is out of range for unsigned char
};

int main() {
    return 0;
}
