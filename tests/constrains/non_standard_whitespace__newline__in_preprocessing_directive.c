// Test 1: Non-standard whitespace (newline) in preprocessing directive

#define PI \ 
3.14159 // Error: Newline between preprocessing tokens is not allowed

int main() {
    PI;
    return 0;
}
