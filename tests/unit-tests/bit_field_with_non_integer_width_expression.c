// Test 7: Bit-field with non-integer width expression

struct TestStruct {
    int field : 3.5; //lint 1270
};

int main() {
    return 0;
}
