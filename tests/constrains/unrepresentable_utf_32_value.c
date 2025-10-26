// Test 8: Unrepresentable UTF-32 value

typedef typeof(U' ') char32_t;
int main() {
    char32_t c = U'\U00110000'; // Out-of-range for UTF-32
    return 0;
}
