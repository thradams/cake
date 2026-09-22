// Test 3: Struct containing an instance of itself

struct TestStruct {
    struct TestStruct self; //lint 740
};

int main() {
    return 0;
}
