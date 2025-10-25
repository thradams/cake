// Test 7: Missing closing parenthesis in macro invocation

#define DOUBLE(x) ((x) * 2)

int main() {
    DOUBLE(5; // Error: Missing closing parenthesis in macro invocation
    return 0;
}
