// Test 4: Invalid preprocessor parameter

#define CONCAT(a, b) a ##@INVALID b // Error: ##@INVALID is not a valid preprocessor parameter

int main() {
    CONCAT(123, 456);
    return 0;
}
