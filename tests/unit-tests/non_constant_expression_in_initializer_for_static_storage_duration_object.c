// Test 5: Non-constant expression in initializer for static storage duration object

int non_const = 5;
static int x = non_const; //lint 1860 requires a compile time object

int main() {
    return 0;
}
