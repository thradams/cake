// Test 5: Bit-field width exceeds type width

struct TestStruct {
    short field : 32; // Error: Bit-field width exceeds type width
};

int main() {
    return 0;
}
