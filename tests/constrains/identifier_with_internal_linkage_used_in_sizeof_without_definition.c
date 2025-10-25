// Test 5: Identifier with internal linkage used in sizeof without definition

static int internal_var; // Declaration without definition

int main() {
    int size = sizeof(internal_var); // No error: sizeof does not require a definition
    return 0;
}
