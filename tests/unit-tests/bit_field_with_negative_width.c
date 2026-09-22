// Test 6: Bit-field with negative width

struct TestStruct {
    int field : -1; //lint 1270 // Error: Bit-field width cannot be negative
};

int main() {
    return 0;
}
