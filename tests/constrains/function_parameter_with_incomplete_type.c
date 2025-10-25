// Test 4: Function parameter with incomplete type


struct Incomplete;

void func_with_incomplete_param(struct Incomplete param) { // Error: Parameter cannot have incomplete type
}
