// Test 4: Function parameter with incomplete type


struct Incomplete;

void func_with_incomplete_param(struct Incomplete param) {
} //lint 740 function has incomplete parameter type
