// Test 4: Alignment specifier in bit-field declaration

struct TestStruct {
    alignas(16) int x : 4; // Error: Alignment specifier cannot be used in bit-field declaration
};

int main() {
    return 0;
}
