// Test 8: Identifier with internal linkage used in typeof without definition

static int typeof_var; // Declaration without definition

int main() {
    typeof(typeof_var) another_var = 5; // No error: typeof does not require a definition
    return 0;
}
