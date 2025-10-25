// Test 8: restrict qualifier in invalid context within struct

struct TestStruct {
    int *restrict ptr; // This is valid
    int restrict not_a_ptr; // Error: restrict qualifier cannot be applied to non-pointer field
};

int main() {
    return 0;
}
