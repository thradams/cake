// Test 7: Applying & to an undefined function designator


int main() {
    void (*func_ptr)() = &undefined_function; // Error: Taking the address of an undeclared function
    return 0;
}
