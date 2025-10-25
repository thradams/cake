// Test 3: Struct containing an instance of itself

struct TestStruct {
    struct TestStruct self; // Error: Struct cannot contain an instance of itself
};

int main() {
    return 0;
}
