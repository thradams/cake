// Test 5: Invalid UTF-32 character literal
typedef typeof(U' ') char32_t;

int main() {
    char32_t c = U'ab'; // UTF-32 literal with more than one character
    return 0;
}
