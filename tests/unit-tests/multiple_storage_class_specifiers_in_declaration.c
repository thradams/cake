// Test 1: Multiple storage-class specifiers in declaration

static extern int x; //lint 2080 at most one storage-class specifier is allowed

int main() {
    return 0;
}
