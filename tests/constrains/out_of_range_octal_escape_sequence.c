// Test 1: Out-of-range octal escape sequence
int main() {
    unsigned char c = '\400'; // Out-of-range for unsigned char
    return 0;
}
