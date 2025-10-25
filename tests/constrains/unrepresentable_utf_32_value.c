// Test 8: Unrepresentable UTF-32 value
int main() {
    char32_t c = U'\U00110000'; // Out-of-range for UTF-32
    return 0;
}
