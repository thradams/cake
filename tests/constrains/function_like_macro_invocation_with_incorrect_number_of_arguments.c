// Test 4: Function-like macro invocation with incorrect number of arguments

#define SUM(a, b) ((a) + (b))

int main() {
    SUM(1); // Error: Incorrect number of arguments in macro invocation
    return 0;
}
