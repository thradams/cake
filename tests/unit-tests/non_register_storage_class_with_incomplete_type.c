// Test 9: Non-register storage class with incomplete type

struct Incomplete;

void invalid_combination(static struct Incomplete param) {
} //lint 740 2290
