// Test 8: Constant expression with side effect in ternary expression

int i;
const int value = (0 ? (i = 10) : 1); // Error: Assignment side effect not allowed in constant expression


