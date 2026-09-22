// Test 6: Identifier with internal linkage used in alignof without definition


static int align_var; // Declaration without definition

int main() {
    int alignment = alignof(align_var); // No error: alignof does not require a definition
    return 0;
}
