// Test 7: Identifier with internal linkage used in generic selection without definition

static int generic_var; // Declaration without definition

int main() {
    int value = _Generic(generic_var, int: 1, default: 0); // No error: generic selection does not require a definition
    return 0;
}
