// Test 8: Bit-field with incompatible type


struct TestStruct {
    float field : 4; // Error: Bit-field type cannot be float
};

int main() {
    return 0;
}
