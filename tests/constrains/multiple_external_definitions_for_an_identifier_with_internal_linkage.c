// Test 3: Multiple external definitions for an identifier with internal linkage

static int static_var = 1;
static int static_var = 2; // Error: Multiple definitions of identifier with internal linkage

int main() {
    return 0;
}
