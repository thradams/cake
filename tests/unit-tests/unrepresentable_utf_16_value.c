typedef typeof(u' ') char16_t;

int main() {
    [[cake::w41]]
    char16_t c = u'\U0001F600'; // Out-of-range for a single UTF-16 code unit
    return 0;
}
