// Test 2: Out-of-range hexadecimal escape sequence
int main() {
    unsigned char c = '\x1FF'; // Out-of-range for unsigned char
    return 0;
}
