// Test 5: Non-constant expression in initializer for static storage duration object

int non_const = 5;
static int x = non_const; // Error: Initializer for static object must be a constant expression

int main() {
    return 0;
}
