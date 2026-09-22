// Test 4: Identifier with internal linkage used in expression without definition

static int static_var; // Declaration without definition

int main() {
    int result = static_var + 5; // Error: Used in expression but not defined
    return 0;
}
