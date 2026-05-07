// Test 6: Unrepresentable UTF-8 value

typedef typeof(u8' ') char8_t;

int main() {
    
    char8_t c = u8'\U0001F600'; //lint 1360 character not encodable in a single code unit.
    return 0;
}
