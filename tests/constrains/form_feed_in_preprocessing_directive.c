// Test 3: Form feed in preprocessing directive


#define CONSTANT \f 42 // Error: Form feed character between preprocessing tokens is not allowed

int main() {
    CONSTANT;
    return 0;
}
