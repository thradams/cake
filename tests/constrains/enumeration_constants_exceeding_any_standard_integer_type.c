// Test 4: Enumeration constants exceeding any standard integer type

enum HugeEnum {
    CONST_A = 0,
    CONST_B = 9223372036854775808U, // Error: Value exceeds any standard integer type
    CONST_C = -9223372036854775809 // Error: Value exceeds any standard integer type
};

int main() {
    return 0;
}
