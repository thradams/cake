// Test 8: restrict qualifier in invalid context within struct

struct TestStruct {
    int *restrict ptr; // This is valid
    int restrict not_a_ptr; //lint 2230
};

int main() {
    return 0;
}
