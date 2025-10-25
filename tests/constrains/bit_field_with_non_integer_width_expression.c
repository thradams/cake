// Test 7: Bit-field with non-integer width expression

struct TestStruct {
    int field : 3.5; // Error: Bit-field width must be an integer constant expression
};

int main() {
    return 0;
}
