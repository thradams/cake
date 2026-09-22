// Test 4: Alignment specifier in bit-field declaration

struct TestStruct {
    alignas(16) int x : 4; //lint 2110 alignment specifier cannot be used in a bit-field declaration
};

int main() {
    return 0;
}
