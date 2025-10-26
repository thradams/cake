// Test 7: Unrepresentable UTF-16 value

typedef typeof(u' ') char16_t;

int main() {
    char16_t c = u'\U0001F600'; // Out-of-range for a single UTF-16 code unit
    return 0;
}
