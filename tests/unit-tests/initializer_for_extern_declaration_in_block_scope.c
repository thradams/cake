// Test 6: Initializer for extern declaration in block scope


int main() {
    extern int external_var = 10; //lint 1980 cannot initialize extern variables with block scope
    return 0;
}
