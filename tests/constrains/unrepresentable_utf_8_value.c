// Test 6: Unrepresentable UTF-8 value

typedef typeof(u8' ') char8_t;

int main() {
    char8_t c = u8'\U0001F600'; // Out-of-range for a single UTF-8 code unit
    return 0;
}
