// Test 8: Function parameter as pointer to incomplete type in function definition

struct Incomplete;

void func_with_incomplete_ptr_param(struct Incomplete* param) { // This should not cause an error
    // Pointer to incomplete type is allowed as parameter
}
