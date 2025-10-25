// Test 6: Initializer for extern declaration in block scope


int main() {
    extern int external_var = 10; // Error: extern variable in block scope cannot have an initializer
    return 0;
}
