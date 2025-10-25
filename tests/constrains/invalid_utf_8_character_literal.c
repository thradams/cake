// Test 3: Invalid UTF-8 character literal
int main() {
    char8_t c = u8'ab'; // UTF-8 literal with more than one character
    return 0;
}
