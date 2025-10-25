// Test 10: Function with block scope and non-extern storage-class specifier


int main() {
    static void my_func() { // Error: function with block scope cannot have static specifier
        //printf("Hello\n");
    }
    return 0;
}
