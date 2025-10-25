// Test 1: Non-integer constant expression in conditional inclusion

#if 3.14 // Error: Controlling expression must be an integer constant expression
    #define PI_DEFINED 1
#endif

int main() {
    return 0;
}
